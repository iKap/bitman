RM = rm
CC = gcc
NAME = bm

build: main.c print.c
	$(CC) main.c print.c -o $(NAME)

clean:
	$(RM) *.o

install:
	mkdir -p $(DESTDIR)/usr/bin/
	install -m 0755 $(NAME) $(DESTDIR)/usr/bin/$(NAME)

rebuild: clean build
	
