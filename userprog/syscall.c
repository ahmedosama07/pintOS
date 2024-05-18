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

static void syscall_handler (struct intr_frame *);

static void exit_handler (struct intr_frame *f);
static void exec_handler (struct intr_frame *f);
static void wait_handler (struct intr_frame *f);
static void create_handler (struct intr_frame *f);
static void remove_handler (struct intr_frame *f);
static void open_handler (struct intr_frame *f);
static void filesize_handler (struct intr_frame *f);;
static void read_handler (struct intr_frame *f);
static void write_handler (struct intr_frame *f);
static void seek (struct intr_frame *f);
static void tell (struct intr_frame *f);
static void close_handler (struct intr_frame *f);

void syscall_init(void)
{
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&file_lock);
}

static void
syscall_handler(struct intr_frame *f)
{
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
        seek(f);
        break;
      }
    case SYS_TELL:
    {
        tell(f);
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

void
halt ()
{
    printf ("(halt) begin\n");
    shutdown_power_off ();
}

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

int
create (char *file_name, int initial_size)
{
    lock_acquire (&file_lock);
    int eax = filesys_create (file_name, initial_size);
    lock_release (&file_lock);
    return eax;
}

int
remove (char *file_name)
{
    lock_acquire(&file_lock);
    int eax = filesys_remove(file_name);
    lock_release(&file_lock);
    return eax;
}

int
open (char *file_name)
{
    static unsigned long descriptor = 2;
    int eax = descriptor; 
    lock_acquire (&file_lock);
    struct file *cur = filesys_open (file_name);
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

int
read (int descriptor, char *buffer, unsigned size)
{
  int eax = size;
  if (descriptor == 0)
    {
      while (size--)
      {
        lock_acquire (&file_lock);
        char c = input_getc ();
        lock_release (&file_lock);
        buffer += c;
      }
    }

  struct user_file *user_file = get_file (descriptor);
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

int
write (int descriptor, char *buffer, unsigned size)
{
  int eax = size;
  if (descriptor == 1)
    {
      lock_acquire(&file_lock);
      putbuf(buffer, size);
      lock_release(&file_lock);
    }
  struct user_file *file = get_file(descriptor);
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

int
close (int descriptor)
{
    struct user_file *file = get_file (descriptor);
    if (file != NULL)
    {
        lock_acquire (&file_lock);
        file_close (file->file);
        lock_release (&file_lock);
        list_remove (&file->elem);
        return 1;
    }
    return -1;
}

#pragma region handlers
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

static void
exec_handler (struct intr_frame *f)
{
    char *file_name = (char *)(*((int *)f->esp + 1));
    f->eax = process_execute (file_name);
}

static void
wait_handler (struct intr_frame *f)
{
    if (!is_vm_valid ((int *)f->esp + 1))
        exit (-1);
    tid_t tid = *((int *)f->esp + 1);
    f->eax = process_wait (tid);
}

static void
create_handler (struct intr_frame *f)
{
    char *file_name = (char *)*((int *)f->esp + 1);
    if (!is_vm_valid (file_name))
        exit (-1);
    int initial_size = (unsigned)*((int *)f->esp + 2);
    f->eax = create (file_name, initial_size);
}

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

static void
filesize_handler (struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    struct user_file *file = get_file(descriptor);
    if (file == NULL)
        f->eax = -1;
    else
    {
        lock_acquire(&file_lock);
        f->eax = file_length(file->file);
        lock_release(&file_lock);
    }
}

static void
read_handler (struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    char *buffer = (char *)(*((int *)f->esp + 2));
    if (descriptor == 1 || !is_vm_valid(buffer))
        exit(-1);
    unsigned size = *((unsigned *)f->esp + 3);
    f->eax = read(descriptor, buffer, size);
}

static void
write_handler (struct intr_frame *f)
{
  int descriptor = *((int *)f->esp + 1);
  char *buffer = (char *)(*((int *)f->esp + 2));
  if (!is_vm_valid(buffer) || descriptor == 0)
      exit(-1);
  unsigned size = (unsigned)(*((int *)f->esp + 3));
  f->eax = write(descriptor, buffer, size);
}

static void
seek (struct intr_frame *f)
{
  int descriptor = (int)(*((int *)f->esp + 1));
  unsigned pos = (unsigned)(*((int *)f->esp + 2));
  struct user_file *file = get_file (descriptor);
  if (file == NULL)
      f->eax = -1;
  else
    {
      lock_acquire (&file_lock);
      file_seek (file->file, pos);
      f->eax = pos;
      lock_release (&file_lock);
    }
}

static void
tell (struct intr_frame *f)
{
  int descriptor = (int)(*((int *)f->esp + 1));
  struct user_file *file = get_file (descriptor);
  if (file == NULL)
      f->eax = -1;
  else
    {
      lock_acquire (&file_lock);
      f->eax = file_tell (file->file);
      lock_release (&file_lock);
    }
}

static void
close_handler(struct intr_frame *f)
{
    int descriptor = (int)(*((int *)f->esp + 1));
    if (descriptor < 2)
    {
        // if the target is stdin or stdout
        exit(-1);
    }
    f->eax = close(descriptor);
}
#pragma endregion handlers

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

bool is_vm_valid(void *name)
{
    return name != NULL && is_user_vaddr(name) && pagedir_get_page(thread_current()->pagedir, name) != NULL;
}
