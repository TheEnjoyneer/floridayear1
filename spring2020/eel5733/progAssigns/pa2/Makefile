CC = gcc
CFLAGS = -Wall -c
THREADS = -lpthread

all:
	$(CC) $(THREADS) combiner.c -o combiner

debug:
	$(CC) $(THREADS) combiner.c -g -o combiner 
	
clean:	
	rm combiner
