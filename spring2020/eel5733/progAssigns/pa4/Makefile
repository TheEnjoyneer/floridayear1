CC = gcc
CFLAGS = -Wall
THREADS = -pthread

all:
	$(CC) $(CFLAGS) $(THREADS) combiner.c -o combiner

debug:
	$(CC) $(CFLAGS) $(THREADS) combiner.c -g -o combiner 
	
clean:	
	rm combiner
