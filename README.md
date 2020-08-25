# arduinOS
Use simulated multithreading to run multiple programs written in byte code at the same time. 

### Overview
During the course operating systems 2 I built my own operating system for an Arduino Uno: ArduinOS. The Arduino can be programmed via an IDE in C, but can only run one program at a time. To start another program it has to be flashed again. Also the microcontroller does not have a file system.

I have built an "operating system" that can run multiple programs at the same time (multitasking), and has a file system in which files can be stored even if the power supply fails. The OS runs programs consisting of byte code.

### Core features
* CLI
* Filesystem
* Run programs written in byte code
* Support for running multipe processes
* Memory management to store process variables

### Available CLI commands
| Command                         | Explanation                 |
| --------------------------------|---------------------------- |
| **store** *filename size data*  | store a file on the EEPROM  |
| **retrieve** *file*             | print the contents of a file|
| **erase** *file*                | erase a file                |
| **files**                       | print all files             |
| **freespace**                   | print space left            |
| **run** *file*                  | run a program from a file   |
| **list**                        | list all processes          |
| **suspend** *pid*               | suspend a process           |
| **resume** *pid*                | resume a process            |
| **kill** *pid*                  | kill a process              |            
