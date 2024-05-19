# Folder structure
Here is a brief overview of the files in the ***threads*** directory.

- [**fixed-point.h**](fixed-point.h):<br>
Implements fixed-point arithmetic for PintOS. See [Fixed-Point RealArithmetic](https://web.stanford.edu/class/cs140/projects/pintos/pintos_7.html#SEC137), for more information.

- [**flags.h**](flags.h):<br>
Macros that define a few bits in the 80x86 "flags" register. Probably of no interest. See [ IA32-v1](https://web.stanford.edu/class/cs140/projects/pintos/pintos_13.html#IA32-v1), section 3.4.3, "EFLAGS Register," for more information.

- [**init.c**](init.c)<br>
  [**init.h**](init.h):<br>
Kernel initialization, including `main()`, the kernel's "main program." See section [A.1.3 High-Level Kernel Initialization](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC94), for details.

- [**interrupt.c**](interrupt.c)<br>
  [**interrupt.h**](interrupt.h):<br>
Interrupt handling and functions for turning interrupts on and off. See section [A.4 Interrupt Handling](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC107), for more information.

- [**intr-stubs.S**](intr-stubs.S)<br>
  [**intr-stubs.h**](intr-stubs.h):<br>
Assembly code for low-level interrupt handling. See section [A.4.1 Interrupt Infrastructure](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC108), for more information.

- [**io.h**](io.h):<br>
Functions for I/O port access. This is mostly used by source code in the devices directory.

- [**kernel.lds.S**](kernel.lds.S):<br>
The linker script used to link the kernel. Sets the load address of the kernel and arranges for [*start.S*](start.S) to be near the beginning of the kernel image. See section [A.1.1 The Loader](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC92), for details.

- [**loader.S**](loader.S)<br>
  [**loader.h**](loader.h):<br>
The kernel loader. Assembles to 512 bytes of code and data that the PC BIOS loads into memory and which in turn finds the kernel on disk, loads it into memory, and jumps to `start()` in [*start.S*](start.S). See section [A.1.1 The Loader](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC92), for details.

- [**malloc.c**](malloc.c)<br>
  [**malloc.h**](malloc.h):<br>
A simple implementation of `malloc()` and `free()` for the kernel. See section [A.5.2 Block Allocator](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC113), for more information.

- [**palloc.c**](palloc.c)<br>
  [**palloc.h**](palloc.h):<br>
Page allocator, which hands out system memory in multiples of 4 kB pages. See section [A.5.1 Page Allocator](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC112), for more information.

- [**pte.h**](pte.h):<br>
Header file defining structures and functions related to page table entries (PTEs).

- [**start.S**](start.S):<br>
Does basic setup needed for memory protection and 32-bit operation on 80x86 CPUs. Unlike the loader, this code is actually part of the kernel. See section [A.1.2 Low-Level Kernel Initialization](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC93), for details.

- [**switch.S**](switch.S)<br>
  [**switch.h**](switch.h):<br>
Assembly language routine for switching threads. Already discussed above. See section [A.2.2 Thread Functions](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC98), for more information.

- [**synch.c**](synch.c)<br>
  [**synch.h**](synch.h):<br>
Synchronization primitives: semaphores, locks, condition variables, and optimization barriers. See section [A.3 Synchronization](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC100), for more information.

- [**thread.c**](thread.c)<br>
  [**thread.h**](thread.h):<br>
Thread support. See [A.2.1 struct thread](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC97) and [A.2 Threads](https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC96) for more information.

- [**vaddr.h**](vaddr.h):<br>
Header file defining structures and functions related to virtual memory addresses (VADDRs).

- [**Make.vars**](Make.vars):<br>
Makefile variables specific to the ***threads*** directory.

- [**Makefile**](Makefile):<br>
Makefile for building the ***threads*** directory.