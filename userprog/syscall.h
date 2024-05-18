#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "stdbool.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

struct lock file_lock;


void syscall_init (void);



void halt();
void exit(int status);
int create(char * file_name,int initial_size);
int remove(char * file_name);
int open(char * file_name);
int read(int descriptor,char* buffer,unsigned size);
int write(int fd,char * buffer,unsigned size);
int close(int fd);

struct user_file *  get_file( int  fd);

bool is_vm_valid (void * name);




#endif /* userprog/syscall.h */
