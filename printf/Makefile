all:
	nasm -f elf64 -o printf.o printf.asm 
	gcc -c main.c -o main.o
	gcc main.o printf.o -no-pie -o test -z noexecstack
