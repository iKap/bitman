RM = del
CC = gcc
NAME = bm

build: main.c print.c
	$(CC) main.c print.c -o $(NAME)

clean:
	!if exist *.o	$(RM) *.o

rebuild: clean build
	
