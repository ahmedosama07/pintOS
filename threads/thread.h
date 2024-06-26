#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h"
#include "threads/fixed-point.h"

static inline int max(int a, int b) {
  return a > b ? a : b;
}

static inline int min(int a, int b) {
  return a < b ? a : b;
}

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */
#define TID_INIT  ((tid_t) -2)          /* Initial value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* Scheduler types. */
#define BSD_SCHEDULER true
#define PRIORITY_SCHEDULER false

/* Mulrilevel priority donations. */
#define PRIORITY_DONATION_MAX_LEVEL 8

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                              /* Thread identifier. */
    enum thread_status status;              /* Thread state. */
    char name[16];                          /* Name (for debugging purposes). */
    uint8_t *stack;                         /* Saved stack pointer. */
    int priority;                           /* Priority. */
    int donated_priority;                   /* Priority to be donated during priority donation.. */
    struct list_elem allelem;               /* List element for all threads list. */
    int64_t wait_ticks;                     /* Number of ticks to be waiting for*/
    int niceness;                           /* Thread nice value, affecting priority calculations.. */
    real_t recent_cpu;                      /* CPU time the thread has used recently, used for scheduling.. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;                  /* List element. */
    struct list waiters;                    /* List of locks held by this thread */
    struct lock *lock_waiting;              /* Lock that this thread is currently waiting for. */
    struct condition *wait_cond;            /* Condition variable that the thread is waiting for. */
    struct semaphore_elem *wait_cond_elem;  /* Semaphore element for the thread waiting on a condition. */
    struct list *sem_list;                  /* List of semaphores held by this thread. */

    

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                      /* Page directory. */
    struct thread *parent;                  /* Parent thread of this thread. */
    struct list children;                   /* List of child threads created by this thread. */
    bool is_child_created;                  /* Flag indicating if a child thread was successfully created. */
    int child_state;                        /* State of the child process. */
    tid_t awaited_tid;                      /* TID of the child thread that this thread is waiting for. */
    struct semaphore child_sema;            /* Semaphore for waiting for child thread to exit. */
    struct semaphore sync;                  /* Semaphore for synchronizing between child thread and parent thread operations. */
    struct list user_files;                 /* List of files opened by this user thread. */
    int exit_code;                          /* Exit code of the thread, used when the thread terminates. */
    struct file *exe;                       /* Executable file of the thread */
    struct list_elem child_elem;            /* List element for this thread in its parent's children list. */
#endif

    /* Owned by thread.c. */
    unsigned magic;                         /* Detects stack overflow. */
  };

struct user_file
  {
    struct list_elem elem;                  /* List element to be used in the list of user files. */
    int descriptor;                         /* File descriptor for the open file. */
    struct file *file;                      /* Pointer to the actual file structure. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

/* Alarm. */
void thread_sleep(int64_t local_thread_ticks);
void thread_awake(int64_t ticks);
bool thread_sleep_compare (const struct list_elem *t1_elem, const struct list_elem *t2_elem, void* UNUSED);
bool thread_priority_compare (const struct list_elem *t1_elem, const struct list_elem *t2_elem, void* UNUSED);
bool is_thread_prior(const struct thread *t1, const struct thread *t2);
void is_time_slice_end(void);

/* Schedulers. */
void threads_update_statistics(bool);
bool locks_priority_comp(const struct list_elem*, const struct list_elem*, void* UNUSED);
void thread_add_to_acquired_locks(struct lock*);
void thread_remove_from_acquired_locks(struct lock*);
void bsd_recalc_priority(void);
void yield_if_not_highest_priority(void);

/* USERPROG. */
#ifdef USERPROG
struct thread *child_with_tid(tid_t tid);
#endif

#endif /* threads/thread.h */
