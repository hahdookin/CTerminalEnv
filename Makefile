all: main.o lexer.o preprocessor.o vector.o utils.o dict.o
	@gcc main.o lexer.o preprocessor.o vector.o utils.o dict.o -o main

debug:
	@gcc -g -std=c99\
		main.c\
		lexer.c\
		preprocessor.c\
		vector.c\
		utils.c\
		dict.c\
		&& gdb -ex "lay src" -ex "file a.out"

clean:
	@rm -f *.o *.out

main.o: main.c
	@gcc -c -std=c99 main.c

lexer.o: lexer.c
	@gcc -c -std=c99 lexer.c

preprocessor.o: preprocessor.c
	@gcc -c -std=c99 preprocessor.c

vector.o: vector.c
	@gcc -c -std=c99 vector.c

dict.o: dict.c
	@gcc -c -std=c99 dict.c

utils.o: utils.c
	@gcc -c -std=c99 utils.c
