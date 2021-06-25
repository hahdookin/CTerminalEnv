all: main.o
	@gcc main.o -o main

debug: main.c
	@gcc -g -std=c99 main.c && gdb -ex "lay src" a.out

main.o: main.c
	@gcc -c -std=c99 main.c
