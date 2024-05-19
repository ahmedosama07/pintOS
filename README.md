# pintOS Project Repository

## Description

PintOS is a lightweight operating system kernel designed for educational purposes. Developed primarily for teaching operating system principles, PintOS provides a practical platform for students to explore concepts such as threads, processes, virtual memory, file systems, and synchronization. With its simple yet comprehensive structure, PintOS offers a hands-on learning experience in understanding the inner workings of an operating system. This repository contains the implementation for the PintOS project.

## Table of Contents

- [Installation](#installation)
- [Project Structure](#project-structure)

## Installation
First, we need to install a simulator, I prefer qemu.
### QEMU Installation
1. Install QEMU on system
    ```
    sudo apt install qemu
    ```
2. Make link `qemu`
    ```
    sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu
    ```

### PintOS Installation
1. Clone this repository to your local machine:

   ```
   git clone https://github.com/ahmedosama07/pintOS
   ```
2. Add `pintos` to PATH
    ```
    echo "export PATH=<path to pintos utils>:$PATH" >> ~/.bashrc
    source ~/.bashrc
    ```
3. Create environment variables for pintos home directory and pintos phase. i.e pintos contains of four phases, threads, userprog, vm and filesys.<br>
    i.e. Add these lines to `~\.bashrc`, or `~\.zshrc` if you use `zsh`, replace `<path to pintos home>` with the actual path to pintOS
    ```
    export PINTOS_HOME=<path to pintos home>
    PINTOSPH="filesys" # stop@000001
    export PINTOSPH

    ```
    you can do so by running these commands
    ```
    echo "export PINTOS_HOME=<path to pintos home>" >> ~/.bashrc
    ```
    ```
    echo "PINTOSPH="threads" # stop@000001" >> ~/.bashrc
    ```
    ```
    echo "export PINTOSPH" >> ~/.bashrc
    ```
    ```
    source ~/.bashrc
    ```
    To validate your work, run:
    ```
    which pintos
    ```
    You should see something like:
    ```
    $PINTOS_HOME/utils/pintos
    ```
4. To change phase, replace `<phase>` with the current phase name.
    ```
    pintosph <phase>
    ```
5. To test for all phases run [`check.sh`](check.sh) using shell name.
    - For `bash`:
    ```
    bash check.sh
    ```
    - For `zsh`:
    ```
    zsh check.sh
    ```

## Project Structure
- [**devices/**](devices/): Contains device drivers and hardware interaction code, including timer, console, and disk drivers.

- [**doc/**](doc/): Contains documentation for the PintOS project, including design documents, user guides, and developer notes.

- [**examples/**](examples/): Contains example user programs that can be run on PintOS to demonstrate various features and test the system.

- [**filesys/**](filesys/): Provides the file system implementation, including files and directories management, buffer cache, and file system utilities.

- [**lib/**](lib/): Provides library code used by both the kernel and user programs. This includes standard C library functions and other utility routines.

- [**misc/**](misc/): Contains miscellaneous files and resources that do not fit into the other directorject consisies. This may include documentation, build scripts, and other auxiliary files.

- [**tests/**](tests/): Includes a suite of tests for verifying the functionality and correctness of different components of PintOS. This is crucial for ensuring reliability and stability as the project develops.

- [**threads/**](threads/): Contains the implementation of kernel threads, synchronization primitives (like semaphores, locks, and condition variables), and thread scheduling.

- [**userprog/**](userprog/): Implements user programs, system calls, and process management. This includes loading and running user programs, handling user-kernel mode transitions, and system call implementations.

- [**utils/**](utils/): Contains utility scripts and tools that assist in building, testing, and managing the PintOS project.

- [**vm/**](vm/): Implements virtual memory management, including paging, swapping, and page fault handling.

- [**LICENSE**](LICENSE): The license file for the PintOS project, detailing the terms under which the project's code can be used, modified, and distributed.

- [**Make.config**](Make.config): Configuration file for the build system, allowing customization of the build process.

- [**Makefile**](Makefile): The main makefile that directs the overall build process, including dependencies and build targets.

- [**Makefile.build**](Makefile.build): A makefile that defines the rules for compiling and linking the project.

- [**Makefile.kernel**](Makefile.kernel): A makefile specifically for building the kernel components of PintOS.

- [**Makefile.userprog**](Makefile.userprog): A makefile specifically for building the user program components of PintOS.

- [**check.sh**](check.sh): A shell script that switches between phases and runs the grading for all phases of PintOS, printing out the summary of results for each phase.

- [**pintos.checker.js**](pintos.checker.cjs): JavaScript script primarily used by GitHub Actions. It fetches the results from an action that grades PintOS phases.