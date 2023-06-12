CC = mpicc
CFLAGS = -fopenmp -Wall -Wextra -pedantic -g

all: docs-mpi

docs-mpi: hash.o docs-mpi.o
	$(CC) $(CFLAGS) -o docs-mpi hash.o docs-mpi.o -lm

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

docs-mpi.o: docs-mpi.c hash.h
	$(CC) $(CFLAGS) -c docs-mpi.c

clean:
	rm -f *.o docs-mpi