#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"

typedef int pid_t;

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct process_control_block
{
    pid_t pid;
    struct thread *parent;
    struct list_elem elem;
    char *commands;

    bool waiting;
    bool exited;
    bool orphan;
    int exitcode;

    struct semaphore sema_initialization;  
    struct semaphore sema_wait;
};

struct file_descriptor
{
  int id;
  struct list_elem elem;
  struct file* file;
};
#endif /* userprog/process.h */
