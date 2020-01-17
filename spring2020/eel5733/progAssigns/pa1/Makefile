CC = gcc
CFLAGS = -c

all:
	$(CC) mapper.c -o mapper
	$(CC) reducer.c -o reducer
	$(CC) combiner.c -o combiner

mapper: mapper.c
	$(CC) $(CFLAGS) mapper.c
	$(CC) mapper.c -o mapper

reducer: reducer.c
	$(CC) $(CFLAGS) reducer.c
	$(CC) reducer.c -o reducer

combiner: combiner.c
	$(CC) $(CFLAGS) combiner.c
	$(CC) combiner.c -o combiner
	
clean:	
	rm mapper reducer combiner
