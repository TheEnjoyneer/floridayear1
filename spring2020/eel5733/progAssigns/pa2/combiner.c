/* combiner.c
 * Christopher Brant
 * Programming Assignment 2
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 2/10/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TUPLE_STRING 27

// Declare dynamic data structure here
struct tupleBuffer_s {
	char **tupleBuf;
	int lastIdx;
	pthread_mutex_t mtx;
	pthread_cond_t full;
};

// Define the mapper thread function here
void *mapperThread(void *arg)
{






	return NULL;
}

// Define the reducer thread function here
void *reducerThread(void *arg)
{






	return NULL;
}


// Main function
int main(int argc, char *argv[])
{
	// Declare and initialize necessary variables
	int i, j;
	int bufSlots = atoi(argv[1]);
	int numBufs = atoi(argv[2]);
	bool prodDone = false;

	// Declare pthreads_t array
	pthreads_t threads[numBufs];

	// Create number of tuple buffers that are necessary
	struct tupleBuffer_s *reducers = (struct tupleBuffer_s *)malloc(sizeof(tupleBuffer_s) * numBufs);
	// Loop through and allocate and initialize values for tupleBuffer structures
	for (i = 0; i < numBufs; i++)
	{
		reducers[i].lastIdx = 0;
		reducers[i].full = PTHREAD_COND_INITIALIZER;
		reducers[i].mtx = PTHREAD_MUTEX_INITIALIZER;
		reducers[i].tupleBuf = (char **)malloc(sizeof(char *) * bufSlots);

		// Loop through and create buffers for tuples
		for (j = 0; j < bufSlots; j++)
			reducers[i].tupleBuf[j] = (char *)malloc(sizeof(char) * TUPLE_STRING);
	}

	// Create all of the threads necessary
	for (i = 0; i < numBufs; i++)
		pthread_create(&threads[i], NULL, reducerThread, NULL);
	// Make the producer thread
	pthread_create(&threads[numBufs], NULL, mapperThread, NULL);





	// What here? All the rest of the threads are running...






	return 0;
}