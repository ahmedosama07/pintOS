# Folder structure
Here is a brief overview of the files in the ***filesys*** directory.

- [**directory.h**](directory.h)<br>
  [**directory.c**](directory.c):<br>
Translates file names to inodes. The directory data structure is stored as a file.

- [**file.h**](file.h)<br>
  [**file.c**](file.c):<br>
Translates file reads and writes to disk sector reads and writes.

- [**filesys.h**](filesys.h)<br>
  [**filesys.c**](filesys.c):<br>
Top-level interface to the file system. See section [3.1.2 Using the File System](https://web.stanford.edu/class/cs140/projects/pintos/pintos_3.html#SEC35), for an introduction.

- [**free-map.h**](free-map.h)<br>
  [**free-map.c**](free-map.c):<br>
Implementation of the free map data structure used for managing free disk blocks.

- [**fsutil.h**](fsutil.h)<br>
  [**fsutil.c**](fsutil.c):<br>
Simple utilities for the file system that are accessible from the kernel command line.

- [**inode.h**](inode.h)<br>
  [**inode.c**](inode.c):<br>
Manages the data structure representing the layout of a file's data on disk.

- [**off_t.h**](off_t.h):<br>
Definition of the off_t type for file offsets.

- [**Make.vars**](Make.vars):<br>
Makefile variables specific to the filesys directory.

- [**Makefile**](Makefile):<br>
Makefile for building the filesys directory.