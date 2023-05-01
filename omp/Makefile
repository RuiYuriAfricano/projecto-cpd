CC=gcc
CFLAGS=-fopenmp -Wall -Wextra -pedantic -g

docs-omp: list.o docs-omp.o
	$(CC) $(CFLAGS) -o principal list.o docs-omp.o -lm

list.o: list.c list.h
	gcc -g -c list.c

docs-omp.o: docs-omp.c list.h
	gcc -g -c docs-omp.c

clean: 
	rm -f *.o principal 
