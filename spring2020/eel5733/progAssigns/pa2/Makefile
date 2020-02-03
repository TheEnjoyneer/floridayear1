CC = gcc
CFLAGS = -Wall -c
THREADS = -lpthread

all:
	$(CC) $(CFLAGS) $(THREADS) combiner.c
	$(CC) $(THREADS) combiner.c -o combiner
	
clean:	
	rm combiner combiner.o
