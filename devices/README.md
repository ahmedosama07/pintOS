# Folder structure
Here is a brief overview of the files in the ***devices*** directory.

- [**block.c**](block.c)<br>
  [**block.h**](block.h):<br>
An abstraction layer for block devices, that is, random-access, disk-like devices that are organized as arrays of fixed-size blocks. Out of the box, Pintos supports two types of block devices: IDE disks and partitions.

- [**ide.c**](ide.c)<br>
  [**ide.h**](ide.h):<br>
Supports reading and writing sectors on up to 4 IDE disks.

- [**input.c**](input.c)<br>
  [**input.h**](input.h):<br>
Input layer. Queues input characters passed along by the keyboard or serial drivers.

- [**intq.c**](intq.c)<br>
  [**intq.h**](intq.h):<br>
Interrupt queue, for managing a circular queue that both kernel threads and interrupt handlers want to access. Used by the keyboard and serial drivers.

- [**kbd.c**](kbd.c)<br>
  [**kbd.h**](kbd.h):<br>
Keyboard driver. Handles keystrokes passing them to the input layer.

- [**partition.c**](partition.c)<br>
  [**partition.h**](partition.h):<br>
Understands the structure of partitions on disks, allowing a single disk to be carved up into multiple regions (partitions) for independent use.

- [**pit.c**](pit.c)<br>
  [**pit.h**](pit.h):<br>
Code to configure the 8254 Programmable Interrupt Timer. This code is used by both devices/timer.c and devices/speaker.c because each device uses one of the PIT's output channel.

- [**rtc.c**](rtc.c)<br>
  [**rtc.h**](rtc.h):<br>
Real-time clock driver, to enable the kernel to determine the current date and time. By default, this is only used by thread/init.c to choose an initial seed for the random number generator.

- [**serial.c**](serial.c)<br>
  [**serial.h**](serial.h):<br>
Serial port driver. It handles serial input by passing it to the input layer

- [**shutdown.c**](shutdown.c)<br>
  [**shutdown.h**](shutdown.h):<br>
Implements shutdown functionality.

- [**speaker.c**](speaker.c)<br>
  [**speaker.h**](speaker.h):<br>
Driver that can produce tones on the PC speaker.

- [**timer.c**](timer.c)<br>
  [**timer.h**](timer.h):<br>
System timer that ticks, by default, 100 times per second.


- [**vga.c**](vga.c)<br>
  [**vga.h**](vga.h):<br>
VGA display driver. Responsible for writing text to the screen.
