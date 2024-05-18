#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "devices/shutdown.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "lib/kernel/list.h"

struct lock file_lock;

static void syscall_handler (struct intr_frame *);

/* System call handlers.*/
static void exit_handler (struct intr_frame *f);
static void exec_handler (struct intr_frame *f);
static void wait_handler (struct intr_frame *f);
static void create_handler (struct intr_frame *f);
static void remove_handler (struct intr_frame *f);
static void open_handler (struct intr_frame *f);
static void filesize_handler (struct intr_frame *f);;
static void read_handler (struct intr_frame *f);
static void write_handler (struct intr_frame *f);
static void seek_handler (struct intr_frame *f);
static void tell_handler (struct intr_frame *f);
static void close_handler (struct intr_frame *f);

/* Helper functions. */
struct user_file *get_file( int  descriptor);
bool is_vm_valid (void * name);

/* Initializes the system call interface by registering the system call interrupt handler.
   This function sets up the interrupt vector to handle system calls (0x30) and
   initializes the file lock used for file system operations. */
void syscall_init(void)
{
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&file_lock);
}

/* Handles incoming system calls by dispatching them to their respective handler functions.
   This function validates the system call number, ensures valid memory access for the syscall arguments,
   and switches control to the appropriate handler based on the syscall number. */
static void
syscall_handler(struct intr_frame *f)
{
  /* Validate system call number and memory access for syscall arguments. */
  if (!is_vm_valid((int *)f->esp) && ((*(int *)f->esp) >= 0) && (*(int *)f->esp) <= 12)
    {
      exit(-1);
    }

  switch (*(int *)f->esp)
    {
    case SYS_HALT:
      {
        halt();
        break;
      }
    case SYS_EXIT:
      {
        exit_handler(f);
        break;
      }
    case SYS_EXEC:
      {
        exec_handler(f);
        break;
      }
    case SYS_WAIT:
      {
        wait_handler(f);
        break;
      }
    case SYS_CREATE:
      {
        create_handler(f);
        break;
      }
    case SYS_REMOVE:
      {
        remove_handler(f);
        break;
      }
    case SYS_OPEN:
      {
        open_handler(f);
        break;
      }
    case SYS_FILESIZE:
      {
        filesize_handler(f);
        break;
      }
    case SYS_READ:
      {
        read_handler(f);
        break;
      }
    case SYS_WRITE:
      {
        write_handler(f);
        break;
      }
    case SYS_SEEK:
      {
        seek_handler(f);
        break;
      }
    case SYS_TELL:
      {
        tell_handler(f);
        break;
      }
    case SYS_CLOSE:
      {
        close_handler(f);
        break;
      }
    
    default:
      {
        NOT_REACHED();
      }
    }
}

/* Halt System Call:
   Shuts down the system after printing a message.
   This function gracefully halts the system by calling shutdown_power_off(). */
void
halt ()
{
    printf ("(halt) begin\n");
    shutdown_power_off ();
}

/* Exit System Call:
   Terminates the current user program, printing an exit message.
   This function sets the exit code for the current thread and terminates it using thread_exit(). */
void
exit (int status)
{
    char *name = thread_current ()->name;
    char *save_ptr;
    char *executable = strtok_r (name, " ", &save_ptr);
    thread_current ()->exit_code = status;
    printf ("%s: exit(%d)\n", executable, status);
    thread_exit ();
}

/* Exec Function:
   Initiates the execution of a new process by parsing and executing the provided command line.
   This function calls process_execute() with the provided command line and returns the process ID (PID) of the new process. */
pid_t
exec (const char* cmd_line) 
{
  return process_execute (cmd_line);
}

/* Wait Function:
   Waits for the child process with the specified process ID (PID) to terminate and retrieves its exit status.
   This function calls process_wait() with the provided process ID and returns the exit status of the child process. */
int
wait (pid_t pid) 
{
  return process_wait (pid);
}

/* Create System Call:
   Creates a new file with the given name and initial size.
   This function acquires the file lock, calls filesys_create(), and releases the lock afterwards. */
bool
create (const char *file, int initial_size)
{
    lock_acquire (&file_lock);
    int eax = filesys_create (file, initial_size);
    lock_release (&file_lock);
    return eax;
}

/* Remove System Call:
   Removes the file with the given name.
   This function acquires the file lock, calls filesys_remove(), and releases the lock afterwards. */
bool
remove (const char *file)
{
    lock_acquire(&file_lock);
    int eax = filesys_remove(file);
    lock_release(&file_lock);
    return eax;
}

/* Open System Call:
   Opens the file with the given name, returning its descriptor.
   This function acquires the file lock, calls filesys_open(), manages file descriptors, and releases the lock. */
int
open (const char *file)
{
    static unsigned long descriptor = 2;
    int eax = descriptor; 
    lock_acquire (&file_lock);
    struct file *cur = filesys_open (file);
    lock_release (&file_lock);

    if (cur == NULL)
        eax = -1;
    else
    {
        struct user_file *file = (struct user_file *)malloc (sizeof (struct user_file));
        file->descriptor = descriptor;
        file->file = cur;
        lock_acquire (&file_lock);
        descriptor++;
        lock_release (&file_lock);
        struct list_elem *elem = &file->elem;
        list_push_back (&thread_current ()->user_files, elem);
    }
    return eax;
}

/* Filesize Function:
   Retrieves the size of the file associated with the given file descriptor.
   This function gets the file information for the descriptor, locks access to the file,
   retrieves the file size using file_length(), and releases the lock.
   It returns the size of the file. */
int
filesize (int fd)
{
  struct user_file *file = get_file(fd);
  int size;
  if (file == NULL)
        size = -1;
    else
    {
        lock_acquire(&file_lock);
        size = file_length(file->file);
        lock_release(&file_lock);
    }
    return size;
}

/* Read System Call:
   Reads from the file descriptor into the buffer.
   This function handles reading from standard input (descriptor 0) and file reads. */
int
read(int fd, void* buffer, unsigned size) 
{
  int eax = size;
  if (fd == STDIN_FILENO)
    {
      while (size--)
      {
        lock_acquire (&file_lock);
        char c = input_getc ();
        lock_release (&file_lock);
        buffer += c;
      }
    }

  struct user_file *user_file = get_file (fd);
  if (user_file == NULL)
      eax = -1;
  else
    {
      struct file *file = user_file->file;
      lock_acquire (&file_lock);
      size = file_read (file, buffer, size);
      lock_release (&file_lock);
    }
  return eax;
}

/* Write System Call:
   Writes to the file descriptor from the buffer.
   This function handles writing to standard output (descriptor 1) and file writes. */
int
write (int fd, const void *buffer, unsigned size)
{
  int eax = size;
  if (fd == STDOUT_FILENO)
    {
      lock_acquire(&file_lock);
      putbuf(buffer, size);
      lock_release(&file_lock);
    }
  struct user_file *file = get_file(fd);
  if (file == NULL)
    eax = -1;
  else
    {
      lock_acquire(&file_lock);
      eax = file_write(file->file, buffer, size);
      lock_release(&file_lock);
    }
    return eax;
}

/* Seek Function:
   Sets the current position of the file represented by the given file descriptor to the specified position.
   This function gets the file information for the descriptor and calls file_seek() to set the position.
   It does not return a value. */
void
seek (int fd, unsigned position)
{
  struct user_file *file = get_file (fd);
  file_seek (file->file, position);
}

/* Tell Function:
   Retrieves the current position within the file represented by the given file descriptor.
   This function initializes the position variable to -1, gets the file information for the descriptor,
   locks access to the file, retrieves the current position using file_tell(), releases the lock,
   and returns the current position within the file. */
unsigned
tell (int fd)
{
  unsigned position = -1;
  struct user_file *file = get_file (fd);
  if (file == NULL)
      position = -1;
  else
    {
      lock_acquire (&file_lock);
      position = file_tell (file->file);
      lock_release (&file_lock);
    }
  return position;
}

/* Close System Call:
   Closes the file associated with the given descriptor.
   This function acquires the file lock, closes the file, and releases the lock. */
void
close (int fd)
{
    struct user_file *file = get_file (fd);
    if (file != NULL)
    {
        lock_acquire (&file_lock);
        file_close (file->file);
        lock_release (&file_lock);
        list_remove (&file->elem);
    }
}

#pragma region handlers

/* Exit Handler:
   Handles the exit system call, terminating the current user program with the given status.
   This function verifies the validity of the status argument, sets the exit code, and terminates the thread. */
static void
exit_handler (struct intr_frame *f)
{
    int status = *((int *)f->esp + 1);
    if (!is_user_vaddr (status))
    {
        f->eax = -1;
        exit (-1);
    }
    f->eax = status;
    exit (status);
}

/* Exec Handler:
   Handles the exec system call, loading and running a new user program.
   This function extracts the file name argument from the interrupt frame stack pointer,
   and calls process_execute() to load and execute the new program. */
static void
exec_handler (struct intr_frame *f)
{
    char *file_name = (char *)(*((int *)f->esp + 1));
    f->eax = exec (file_name);
}

/* Wait Handler:
   Handles the wait system call, waiting for a child process to terminate and retrieving its exit status.
   This function verifies the validity of the tid argument, and calls process_wait() to wait for the child process.
   It sets the return value of the wait system call to the exit status of the child process. */
static void
wait_handler (struct intr_frame *f)
{
    if (!is_vm_valid ((int *)f->esp + 1))
        exit (-1);
    tid_t tid = *((int *)f->esp + 1);
    f->eax = wait (tid);
}

/* Create Handler:
   Handles the create system call, creating a new file with the given name and initial size.
   This function verifies the validity of the file name argument, and calls create() to create the file.
   It sets the return value of the create system call to the result of the create() function. */
static void
create_handler (struct intr_frame *f)
{
    char *file_name = (char *)*((int *)f->esp + 1);
    if (!is_vm_valid (file_name))
        exit (-1);
    int initial_size = (unsigned)*((int *)f->esp + 2);
    f->eax = create (file_name, initial_size);
}

/* Remove Handler:
   Handles the remove system call, deleting the file with the given name.
   This function verifies the validity of the file name argument, and calls remove() to delete the file.
   It sets the return value of the remove system call to the result of the remove() function. */
static void
remove_handler (struct intr_frame *f)
{
    char *file_name = (char *)(*((int *)f->esp + 1));
    if (!is_vm_valid (file_name))
    {
        exit (-1);
    }
    f->eax = remove (file_name);
}

/* Open Handler:
   Handles the open system call, opening the file with the given name.
   This function verifies the validity of the file name argument and calls open() to open the file.
   It sets the return value of the open system call to the result of the open() function. */
static void
open_handler (struct intr_frame *f)
{
    char *file_name = (char *)(*((int *)f->esp + 1));
    if (!is_vm_valid (file_name))
    {
        exit (-1);
    }
    f->eax = open (file_name);
}

/* Filesize Handler:
   Handles the filesize system call, retrieving the size of the file associated with the given descriptor.
   This function verifies the validity of the descriptor argument, retrieves the file information, and calls file_length() to get the file size.
   It sets the return value of the filesize system call to the size of the file. */
static void
filesize_handler (struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    f->eax = filesize (descriptor);
}

/* Read Handler:
   Handles the read system call, reading data from the file descriptor into the buffer.
   This function verifies the validity of the descriptor and buffer arguments,
   and calls the read() function to perform the actual read operation.
   It sets the return value of the read system call to the result of the read() function. */
static void
read_handler (struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    char *buffer = (char *)(*((int *)f->esp + 2));
    if (descriptor == STDOUT_FILENO || !is_vm_valid(buffer))
        exit(-1);
    unsigned size = *((unsigned *)f->esp + 3);
    f->eax = read(descriptor, buffer, size);
}

static void
write_handler (struct intr_frame *f)
{
  int descriptor = *((int *)f->esp + 1);
  char *buffer = (char *)(*((int *)f->esp + 2));
  if (!is_vm_valid(buffer) || descriptor == STDIN_FILENO)
      exit(-1);
  unsigned size = (unsigned)(*((int *)f->esp + 3));
  f->eax = write(descriptor, buffer, size);
}

static void
seek_handler (struct intr_frame *f)
{
  int descriptor = (int)(*((int *)f->esp + 1));
  unsigned pos = (unsigned)(*((int *)f->esp + 2));
  struct user_file *file = get_file (descriptor);
  if (file == NULL)
      f->eax = -1;
  else
    {
      lock_acquire (&file_lock);
      seek (descriptor, pos);
      f->eax = pos;
      lock_release (&file_lock);
    }
}

static void
tell_handler (struct intr_frame *f)
{
  int descriptor = (int)(*((int *)f->esp + 1));
  f->eax = tell (descriptor);
}

static void
close_handler(struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    struct user_file *file = get_file (descriptor);
    if (descriptor < 2)
    {
        // if the target is stdin or stdout
        exit(-1);
    }
    f->eax = (file != NULL ? 1 : -1);

}
#pragma endregion handlers

/* Get File Function:
   Retrieves the user_file structure associated with the given file descriptor from the current thread's list of open files.
   This function iterates through the list of user_files belonging to the current thread, compares the descriptors,
   and returns the user_file structure if found. If not found, it returns NULL. */
struct user_file *get_file(int descriptor)
{
    struct list *list = &(thread_current()->user_files);
    for (struct list_elem *elem = list_begin(list); elem != list_end(list); elem = list_next(elem))
    {
        struct user_file *file = list_entry(elem, struct user_file, elem);
        if ((file->descriptor) == descriptor)
            return file;
    }
    return NULL;
}

/* Virtual Memory Validation Function:
   Checks whether the given virtual memory address is valid within the current process's address space.
   This function verifies that the address is not NULL, is within the user space address range,
   and that it corresponds to a mapped page in the process's page directory.
   It returns true if the address is valid, otherwise false. */
bool is_vm_valid(void *name)
{
    return name != NULL && is_user_vaddr(name) && pagedir_get_page(thread_current()->pagedir, name) != NULL;
}
