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
3. Create environment variables for pintos home directory and pintos phase. i.e pintos contains of four phases, threads, userprog, vm and filesys.
    ```
    echo "export PINTOS_HOME=~/Documents/GitHub/pintos" >> ~/.bashrc
    echo "PINTOSPH="filesys" # stop@000001" ~/.bashrc
    echo "export PINTOSPH" >> ~/.bashrc
    source ~/.bashrc
    ```
4. To change phase
    ```
    pintosph <phase>
    ```