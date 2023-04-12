CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g

docs-serial: list.o docs-serial.o
	$(CC) $(CFLAGS) -o principal list.o docs-serial.o -lm

list.o: list.c list.h
	gcc -g -c list.c

docs-serial.o: docs-serial.c list.h
	gcc -g -c docs-serial.c

clean: 
	rm -f *.o principal 
