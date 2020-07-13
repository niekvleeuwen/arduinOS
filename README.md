# arduinOS
Use simulated multithreading to run multiple programs written in byte code at the same time. 

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
