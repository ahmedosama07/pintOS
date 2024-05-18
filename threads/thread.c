#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
static struct list ready_list;

/* List of processes in sleep/wait state, that is, processes
   that are paused for number of ticks determined. */
static struct list blocked_list;

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
static struct list all_list;

/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;

/* Stack frame for kernel_thread(). */
struct kernel_thread_frame 
  {
    void *eip;                  /* Return address. */
    thread_func *function;      /* Function to call. */
    void *aux;                  /* Auxiliary data for function. */
  };

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */
static real_t load_avg;         /* Load average. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread *running_thread (void);
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static bool is_thread (struct thread *) UNUSED;
static void *alloc_frame (struct thread *, size_t size);
static void schedule (void);
void thread_schedule_tail (struct thread *prev);
static tid_t allocate_tid (void);


/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void) 
{
  ASSERT (intr_get_level () == INTR_OFF);

  lock_init (&tid_lock);
  list_init (&ready_list);
  list_init (&blocked_list);
  list_init (&all_list);

  /* Set up a thread structure for the running thread. */
  initial_thread = running_thread ();
  init_thread (initial_thread, "main", PRI_DEFAULT);
  initial_thread->status = THREAD_RUNNING;
  initial_thread->tid = allocate_tid ();
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) 
{
  /* Create the idle thread. */
  struct semaphore idle_started;
  sema_init (&idle_started, 0);
  thread_create ("idle", PRI_MIN, idle, &idle_started);

  /* Start preemptive thread scheduling. */
  intr_enable ();

  /* Wait for the idle thread to initialize idle_thread. */
  sema_down (&idle_started);
}

/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) 
{
  struct thread *t = thread_current ();

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
    user_ticks++;
#endif
  else
    kernel_ticks++;

  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
    intr_yield_on_return ();
}

/*Function to sleep theeads the number of ticks needed
at first check if it's not an idle thread then save the ticks in wait_ticks
and insert this thread in blocked_list and block it*/
void
thread_sleep (int64_t local_thread_ticks)
{
  ASSERT(intr_get_level() == INTR_OFF);
  struct thread *t = thread_current();
  t->wait_ticks = local_thread_ticks;
  list_insert_ordered(&blocked_list, &t->elem, &thread_sleep_compare, NULL);
  thread_block();
}

/*Function to periodically check if there's a thread is ready to be awaked after 
its ticks got over by checking each thread in blocked_list and comparing its wait_ticks 
to current ticks and unblocking it*/
void
thread_awake(int64_t ticks)
{
  struct thread *t;
  int max_priority = PRI_MIN;
  while (!list_empty(&blocked_list) && (t = list_entry(list_front(&blocked_list), struct thread, elem)) && t->wait_ticks <= ticks) {
    list_remove(&t->elem);
    thread_unblock(t);
    max_priority = max(max_priority, (t->donated_priority != -1? t->donated_priority: t->priority));
  }
  if(max_priority > thread_get_priority())
    intr_yield_on_return();
}

/* Prints thread statistics. */
void
thread_print_stats (void) 
{
  printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
          idle_ticks, kernel_ticks, user_ticks);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
               thread_func *function, void *aux) 
{
  struct thread *t;
  struct kernel_thread_frame *kf;
  struct switch_entry_frame *ef;
  struct switch_threads_frame *sf;
  tid_t tid;

  ASSERT (function != NULL);

  /* Allocate thread. */
  t = palloc_get_page (PAL_ZERO);
  if (t == NULL)
    return TID_ERROR;

  /* Initialize thread. */
  init_thread (t, name, priority);
  tid = t->tid = allocate_tid ();

  /* Stack frame for kernel_thread(). */
  kf = alloc_frame (t, sizeof *kf);
  kf->eip = NULL;
  kf->function = function;
  kf->aux = aux;

  /* Stack frame for switch_entry(). */
  ef = alloc_frame (t, sizeof *ef);
  ef->eip = (void (*) (void)) kernel_thread;

  /* Stack frame for switch_threads(). */
  sf = alloc_frame (t, sizeof *sf);
  sf->eip = switch_entry;
  sf->ebp = 0;

  /* Add to run queue. */
  t->niceness = thread_current()->niceness;
  thread_unblock (t);

  yield_on_max_priority();

  return tid;
}

/* The initial value of recent_cpu is 0 in the first thread created, or the parent's value in other new threads.
   Each time a timer interrupt occurs, recent_cpu is incremented by 1 for the running thread only, unless the idle thread is running.
   In addition, once per second the value of recent_cpu is recalculated for every thread (whether running, ready, or blocked). */
void threads_update_statistics(bool one_second) {

  if (one_second) {
    uint32_t running_threads_count = list_size(&ready_list) + (thread_current() != idle_thread);
    load_avg = add_real(divide_int(multiply_int(load_avg, 59), 60), divide_int(to_real(running_threads_count), 60));

    struct list_elem* cur_iter = list_head(&all_list);
    struct thread* cur;
    while ((cur_iter = list_next(cur_iter)) != list_tail(&all_list)) {
      cur = list_entry(cur_iter, struct thread, allelem);
      if (cur != idle_thread) {
        real_t recent_cpu = cur->recent_cpu;
        real_t load_avg_double = multiply_int(load_avg, 2);
        cur->recent_cpu = add_int(multiply_real(divide_real(load_avg_double, add_int(load_avg_double, 1)), recent_cpu), cur->niceness);
      }
    }
  }

  if (thread_current() != idle_thread)
    thread_current()->recent_cpu = add_int(thread_current()->recent_cpu, 1);
}

/* Our scheduler has 64 priorities and thus 64 ready queues, numbered 0 (PRI_MIN) through 63 (PRI_MAX). 
   Lower numbers correspond to lower priorities, so that priority 0 is the lowest priority and priority 
   63 is the highest. Thread priority is calculated initially at thread initialization. 
   It is also recalculated once every fourth clock tick, for every thread. */
void bsd_recalc_priority(void) {
  struct list_elem* cur_iter = list_head(&all_list);
  struct thread* cur;
  while ((cur_iter = list_next(cur_iter)) != list_tail(&all_list)) {
    cur = list_entry(cur_iter, struct thread, allelem);
    if (cur == idle_thread)
      continue;
    int new_priority = PRI_MAX - to_int(divide_int(cur->recent_cpu, 4)) - (2 * cur->niceness);
    cur->priority = min(PRI_MAX, max(PRI_MIN, new_priority));
  }
  list_sort(&ready_list, thread_priority_compare, NULL);
}

/* Puts the current thread to sleep.  It will not be scheduled
   again until awoken by thread_unblock().

   This function must be called with interrupts turned off.  It
   is usually a better idea to use one of the synchronization
   primitives in synch.h. */
void
thread_block (void) 
{
  ASSERT (!intr_context ());
  ASSERT (intr_get_level () == INTR_OFF);

  thread_current ()->status = THREAD_BLOCKED;
  schedule ();
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
void
thread_unblock (struct thread *t) 
{
  enum intr_level old_level;

  ASSERT (is_thread (t));

  old_level = intr_disable ();
  ASSERT (t->status == THREAD_BLOCKED);
  list_insert_ordered (&ready_list, &t->elem, thread_priority_compare, NULL);
  t->status = THREAD_READY;
  intr_set_level (old_level);
}

/* Returns the name of the running thread. */
const char *
thread_name (void) 
{
  return thread_current ()->name;
}

/* Returns the running thread.
   This is running_thread() plus a couple of sanity checks.
   See the big comment at the top of thread.h for details. */
struct thread *
thread_current (void) 
{
  struct thread *t = running_thread ();
  
  /* Make sure T is really a thread.
     If either of these assertions fire, then your thread may
     have overflowed its stack.  Each thread has less than 4 kB
     of stack, so a few big automatic arrays or moderate
     recursion can cause stack overflow. */
  ASSERT (is_thread (t));
  ASSERT (t->status == THREAD_RUNNING);

  return t;
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) 
{
  return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) 
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  intr_disable ();
  list_remove (&thread_current()->allelem);
  thread_current ()->status = THREAD_DYING;
  schedule ();
  NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) 
{
  struct thread *cur = thread_current ();
  enum intr_level old_level;
  
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  if (cur != idle_thread)
    list_insert_ordered(&ready_list, &cur->elem, thread_priority_compare, NULL);
  cur->status = THREAD_READY;
  schedule ();
  intr_set_level (old_level);
}

/* Invoke function 'func' on all threads, passing along 'aux'.
   This function must be called with interrupts off. */
void
thread_foreach (thread_action_func *func, void *aux)
{
  struct list_elem *e;

  ASSERT (intr_get_level () == INTR_OFF);

  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      func (t, aux);
    }
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) 
{
  enum intr_level old_level = intr_disable();
  struct thread *cur = thread_current();
  cur->priority = new_priority;
  cur->donated_priority = (cur->donated_priority != -1) ? max(cur->donated_priority, new_priority) : -1;

  if(cur->sem_list != NULL)
    list_insert_ordered(cur->sem_list, &cur->elem, thread_priority_compare, NULL);
  
  if(cur->wait_cond != NULL)
    list_insert_ordered(&cur->wait_cond->waiters, &cur->wait_cond_elem->elem, semaphore_priority_compare, NULL);

  if(!list_empty(&ready_list) && is_thread_prior(list_entry(list_front(&ready_list), struct thread, elem), cur))
  {
    thread_yield();
  }
  intr_set_level(old_level);
}

/* Returns the current thread's priority. */
int
thread_get_priority (void) 
{
  return thread_current()->donated_priority != -1? thread_current()->donated_priority: thread_current()->priority;
}

/* Sets the current thread's nice value to NICE. */
void
thread_set_nice (int nice UNUSED) 
{
  struct thread* cur = thread_current();
  cur->niceness = nice;
  int new_priority = PRI_MAX - to_int(divide_int(thread_current()->recent_cpu, 4)) - (nice * 2);
  thread_current() ->priority = min(PRI_MAX, max(PRI_MIN, new_priority)); 
  yield_on_max_priority();
}

/* Returns the current thread's nice value. */
int
thread_get_nice (void) 
{
  return thread_current()->niceness;
}

/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) 
{
  return round_real(multiply_int(load_avg, 100));
}

/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) 
{
  return round_real(multiply_int(thread_current()->recent_cpu, 100));
}

/* Add thread to acquired lock. */
void thread_add_to_acquired_locks(struct lock* l){
  struct thread *cur = thread_current();
  list_insert_ordered(&cur->waiters, &l->lockelem, locks_priority_comp,NULL);//add new acquired lock in the list of the thread
}

/* Remove acquired lock. */
void thread_remove_from_acquired_locks(struct lock* l){
  struct thread *cur = thread_current();
  ASSERT(!list_empty(&cur->waiters));
  list_remove(&l->lockelem);

  if(list_empty(&cur->waiters))
    cur->donated_priority = -1;
  else
    cur->donated_priority = list_entry(list_front(&cur->waiters), struct lock, lockelem)->donated_priority;
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) 
{
  struct semaphore *idle_started = idle_started_;
  idle_thread = thread_current ();
  sema_up (idle_started);

  for (;;) 
    {
      /* Let someone else run. */
      intr_disable ();
      thread_block ();

      /* Re-enable interrupts and wait for the next one.

         The `sti' instruction disables interrupts until the
         completion of the next instruction, so these two
         instructions are executed atomically.  This atomicity is
         important; otherwise, an interrupt could be handled
         between re-enabling interrupts and waiting for the next
         one to occur, wasting as much as one clock tick worth of
         time.

         See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
         7.11.1 "HLT Instruction". */
      asm volatile ("sti; hlt" : : : "memory");
    }
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) 
{
  ASSERT (function != NULL);

  intr_enable ();       /* The scheduler runs with interrupts off. */
  function (aux);       /* Execute the thread function. */
  thread_exit ();       /* If function() returns, kill the thread. */
}

/* Returns the running thread. */
struct thread *
running_thread (void) 
{
  uint32_t *esp;

  /* Copy the CPU's stack pointer into `esp', and then round that
     down to the start of a page.  Because `struct thread' is
     always at the beginning of a page and the stack pointer is
     somewhere in the middle, this locates the curent thread. */
  asm ("mov %%esp, %0" : "=g" (esp));
  return pg_round_down (esp);
}

/* Returns true if T appears to point to a valid thread. */
static bool
is_thread (struct thread *t)
{
  return t != NULL && t->magic == THREAD_MAGIC;
}

/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority)
{
  enum intr_level old_level;

  ASSERT (t != NULL);
  ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
  ASSERT (name != NULL);
  struct thread* current = (t == initial_thread ? NULL : thread_current());

  memset (t, 0, sizeof *t);
  t->status = THREAD_BLOCKED;
  strlcpy (t->name, name, sizeof t->name);
  t->stack = (uint8_t *) t + PGSIZE;
  t->priority = priority;
  t->donated_priority = -1;
  t->wait_ticks = 0;
  t->lock_waiting = NULL;
  list_init(&t->waiters);
  t->sem_list = NULL;
  t->niceness = (t == initial_thread ? 0: current->niceness);
  t->recent_cpu = (t == initial_thread ? (real_t){0} : current->recent_cpu);
  t->magic = THREAD_MAGIC;

  old_level = intr_disable ();
  list_push_back (&all_list, &t->allelem);
  intr_set_level (old_level);
}

/* Allocates a SIZE-byte frame at the top of thread T's stack and
   returns a pointer to the frame's base. */
static void *
alloc_frame (struct thread *t, size_t size) 
{
  /* Stack data is always allocated in word-size units. */
  ASSERT (is_thread (t));
  ASSERT (size % sizeof (uint32_t) == 0);

  t->stack -= size;
  return t->stack;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
static struct thread *
next_thread_to_run (void) 
{
  if (list_empty (&ready_list))
    return idle_thread;
  else
    return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/* Completes a thread switch by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.  This function is normally invoked by
   thread_schedule() as its final action before returning, but
   the first time a thread is scheduled it is called by
   switch_entry() (see switch.S).

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function.

   After this function and its caller returns, the thread switch
   is complete. */
void
thread_schedule_tail (struct thread *prev)
{
  struct thread *cur = running_thread ();
  
  ASSERT (intr_get_level () == INTR_OFF);

  /* Mark us as running. */
  cur->status = THREAD_RUNNING;

  /* Start new time slice. */
  thread_ticks = 0;

#ifdef USERPROG
  /* Activate the new address space. */
  process_activate ();
#endif

  /* If the thread we switched from is dying, destroy its struct
     thread.  This must happen late so that thread_exit() doesn't
     pull out the rug under itself.  (We don't free
     initial_thread because its memory was not obtained via
     palloc().) */
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread) 
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
}

/* Schedules a new process.  At entry, interrupts must be off and
   the running process's state must have been changed from
   running to some other state.  This function finds another
   thread to run and switches to it.

   It's not safe to call printf() until thread_schedule_tail()
   has completed. */
static void
schedule (void) 
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) 
{
  static tid_t next_tid = 1;
  tid_t tid;

  lock_acquire (&tid_lock);
  tid = next_tid++;
  lock_release (&tid_lock);

  return tid;
}

/*To insert sleeping threads ordered using list_insert_ordered 
as it needs list_less_func * to compare the waiting time and insert*/
bool
thread_sleep_compare (const struct list_elem *t1_elem, const struct list_elem *t2_elem, void *aux UNUSED)
{
  ASSERT(t1_elem != NULL && t2_elem != NULL);
  const struct thread *t1 = list_entry (t1_elem, struct thread, elem);
  const struct thread *t2 = list_entry (t2_elem, struct thread, elem);

  if(t1->wait_ticks == t2->wait_ticks)
     return is_thread_prior(t1, t2);
  return t1->wait_ticks < t2->wait_ticks;
}

/*To insert ready threads ordered using list_insert_ordered 
as it needs list_less_func * to compare the priority and insert*/
bool
thread_priority_compare (const struct list_elem *t1_elem, const struct list_elem *t2_elem, void *aux UNUSED)
{
  ASSERT(t1_elem != NULL && t2_elem != NULL);
  const struct thread *t1 = list_entry (t1_elem, struct thread, elem);
  const struct thread *t2 = list_entry (t2_elem, struct thread, elem);

  return is_thread_prior(t1, t2);
}

/* Return true if lock l1 priority is less than lock l2 priority. */
bool 
locks_priority_comp(const struct list_elem *l1_elem, const struct list_elem *l2_elem, void *aux UNUSED)
{
  ASSERT(l1_elem != NULL && l2_elem != NULL);
  struct lock *l1 = list_entry(l1_elem, struct lock, lockelem);
  struct lock *l2 = list_entry(l2_elem, struct lock, lockelem);
  return l1->donated_priority > l2->donated_priority;
}


void yield_on_max_priority(void)
{
  if (intr_context()) return;
  enum intr_level old_level = intr_disable();
  if(!list_empty(&ready_list) && is_thread_prior(list_entry(list_front(&ready_list), struct thread, elem), thread_current()))
    thread_yield();
  intr_set_level(old_level);
}

bool
is_thread_prior(const struct thread *t1, const struct thread *t2) {
  int t1_priority = (t1->donated_priority != -1? t1->donated_priority : t1->priority);
  int t2_priority = (t2->donated_priority != -1? t2->donated_priority : t2->priority);
  return (t1_priority == t2_priority? false : t1_priority > t2_priority);
}

inline void is_time_slice_end()
{
  if(thread_ticks == TIME_SLICE)
    intr_yield_on_return();
}




/* Offset of `stack' member within `struct thread'.
   Used by switch.S, which can't figure it out on its own. */
uint32_t thread_stack_ofs = offsetof (struct thread, stack);
