CC = gcc
CFLAGS = -Wall -c
THREADS = -lpthread

all:
	$(CC) mapper.c -o mapper
	$(CC) reducer.c -o reducer
	$(CC) $(THREADS) combiner.c -o combiner

mapper: mapper.c
	$(CC) $(CFLAGS) mapper.c
	$(CC) mapper.c -o mapper

reducer: reducer.c
	$(CC) $(CFLAGS) reducer.c
	$(CC) reducer.c -o reducer

combiner: combiner.c
	$(CC) $(CFLAGS) $(THREADS) combiner.c
	$(CC) $(THREADS) combiner.c -o combiner

mapperClean:
	rm mapper mapper.o

reducerClean:
	rm reducer reducer.o

combinerClean:
	rm combiner combiner.o

allClean:
	rm mapper mapper.o reducer reducer.o combiner combiner.o
	
clean:	
	rm mapper reducer combiner
