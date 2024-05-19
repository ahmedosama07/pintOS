# Folder structure
Here is a brief overview of the files in the ***userprog*** directory.

- [**exception.c**](exception.c)<br>
  [**exception.h**](exception.h):<br>
When a user process performs a privileged or prohibited operation, it traps into the kernel as an "exception" or "fault."[(3)](https://web.stanford.edu/class/cs140/projects/pintos/pintos_fot.html#FOOT3) These files handle exceptions.

- [**gdt.c**](gdt.c)<br>
  [**gdt.h**](gdt.h):<br>
The 80x86 is a segmented architecture. The Global Descriptor Table (GDT) is a table that describes the segments in use. These files set up the GDT. 

- [**pagedir.c**](pagedir.c)<br>
  [**pagedir.h**](pagedir.h):<br>
A simple manager for 80x86 hardware page tables. See section [4.1.2.3 Page Tables](https://web.stanford.edu/class/cs140/projects/pintos/pintos_4.html#SEC59), for more information.

- [**process.c**](process.c)<br>
  [**process.h**](process.h):<br>
Loads ELF binaries and starts processes.

- [**syscall.c**](syscall.c)<br>
  [**syscall.h**](syscall.h):<br>
Whenever a user process wants to access some kernel functionality, it invokes a system call.

- [**tss.c**](tss.c)<br>
  [**tss.h**](tss.h):<br>
The Task-State Segment (TSS) is used for 80x86 architectural task switching. Pintos uses the TSS only for switching stacks when a user process enters an interrupt handler, as does Linux.

- [**Make.vars**](Make.vars):<br>
Makefile variables specific to the ***userprog*** directory.

- [**Makefile**](Makefile):<br>
Makefile for building the ***userprog*** directory.