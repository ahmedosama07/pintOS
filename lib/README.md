# Folder structure
Here is a brief overview of the files in the ***lib*** directory.

- [**ctype.h**](ctype.h)<br>
  [**inttypes.h**](inttypes.h)<br>
  [**limits.h**](limits.h)<br>
  [**stdarg.h**](stdarg.h)<br>
  [**stdbool.h**](stdbool.h)<br>
  [**stddef.h**](stddef.h)<br>
  [**stdint.h**](stdint.h)<br>
  [**stdio.c**](stdio.c)<br>
  [**stdio.h**](stdio.h)<br>
  [**stdlib.c**](stdlib.c)<br>
  [**stdlib.h**](stdlib.h)<br>
  [**string.c**](string.c)<br>
  [**string.h**](string.h):<br>
A subset of the standard C library. See section [C.2 C99](https://web.stanford.edu/class/cs140/projects/pintos/pintos_8.html#SEC140), for information on a few recently introduced pieces of the C library that you might not have encountered before. See section [C.3 Unsafe String Functions](https://web.stanford.edu/class/cs140/projects/pintos/pintos_8.html#SEC141), for information on what's been intentionally left out for safety.

- [**debug.c**](debug.c)<br>
  [**debug.h**](debug.h):<br>
Functions and macros to aid debugging. See section [E. Debugging Tools](https://web.stanford.edu/class/cs140/projects/pintos/pintos_10.html#SEC145), for more information.

- [**random.c**](random.c)<br>
  [**random.h**](random.h):<br>
Pseudo-random number generator. The actual sequence of random values will not vary from one Pintos run to another, unless you do one of three things: specify a new random seed value on the -rs kernel command-line option on each run, or use a simulator other than Bochs, or specify the -r option to pintos.

- [**round.h**](round.h):<br>
Macros for rounding.

- [**syscall-nr.h**](syscall-nr.h):<br>
System call numbers.

- [**kernel/list.c**](kernel/list.c)<br>
  [**kernel/list.h**](kernel/list.h):<br>
Doubly linked list implementation.

- [**kernel/bitmap.c**](kernel/bitmap.c)<br>
  [**kernel/bitmap.h**](kernel/bitmap.h):<br>
Bitmap implementation.

- [**kernel/hash.c**](kernel/hash.c)<br>
  [**kernel/hash.h**](kernel/hash.h):<br>
Hash table implementation.

- [**kernel/console.c**](kernel/console.c)<br>
  [**kernel/console.h**](kernel/console.h)<br>
  [**kernel/stdio.h**](kernel/stdio.h):<br>
Implements printf() and a few other functions.
