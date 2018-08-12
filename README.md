# FileSystem

The program support:
search - When the flag -n <name> is supplied, it will only print the names of the files (with their relative paths) with this specific name.
display of the files in the system - list the relative path from the current directory of all the files in the working directory.
execute commands on files with a specific name - When the flag -e <name> <command> is supplied.

The program prints its arguments to standard output without using the standard C library but,
using the following system calls: open, close, read, write, lseek, exit, and getdents.

helper files: 
start.s - using us a linker with _start and contains the system_call function (meaning i am not useing the syscall in Assembly).
util.c - include helping functions for the program.
