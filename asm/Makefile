CC = gcc
CFLAGS = -g -Wall

automata: automata.o queue.o list.o reduce.asm
	nasm -g -f elf64 reduce.asm
	gcc -g reduce.o automata.o queue.o list.o -o automata

hello:	hello.asm
	nasm -f elf64 hello.asm
	gcc -g hello.o -o hello

read:	read.asm
	nasm -f elf64 read.asm
	gcc -g read.o -o read

stdin:	stdin.o

clean:
	rm *.o hello read automata stdin
