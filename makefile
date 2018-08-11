all: search

search: lab4_start.o main.o lab4_util.o
	ld -m elf_x86_64 lab4_start.o main.o lab4_util.o -o search
main.o: main.c
	gcc -m64 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

lab4_util.o: lab4_util.c
	gcc -m64 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o lab4_util.o

lab4_start.o: lab4_start.s
	nasm -f elf64 lab4_start.s -o lab4_start.o


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o search
