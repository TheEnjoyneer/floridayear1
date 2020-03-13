/* combiner.c
 * Christopher Brant
 * Programming Assignment 4
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 3/14/20
 */

#ifdef USE_MAP_ANON
#define _BSD_SOURCE		// Get MAN_ANONYMOUS definition
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>


// Define all pound define variables
#define P_WEIGHT 50
#define L_WEIGHT 20
#define D_WEIGHT -10
#define C_WEIGHT 30
#define S_WEIGHT 40
#define TOPIC_LENGTH 16
#define INPUT_STR_LEN 25
#define TUPLE_STRING 27
#define BUF_SIZE 30
#define MAX_TOPICS 10


// // Declare static variables
static int bufSlots;
static int numBufs;
static bool *prodDone;

// Declare necessary structures here
struct tuple_s {
	char userID[5];
	char action[2];
	char topic[TOPIC_LENGTH];
	int score;
};

struct tupleBuffer_s {
	char **tupleBuf;		// Array of tuple strings
	int lastIdx;			// Index of the last filled slot of the buffer
	pthread_mutex_t mtx;	// Mutex variable
	pthread_cond_t isFull;	// Condition for when buffer is no longer full
	pthread_cond_t isEmpty;	// Conditioin for when buffer is no longer empty
};


// Declare helper functions
static void stringFormat(char *inputStr, char *outputStr);
static void errorHandler(int threadErr);


// Define the mapper thread function here
void mapperFunc(struct tupleBuffer_s * bufferStructs)
{
	// Declare necessary variables
	int i, threadErr;
	char inputBuf[BUF_SIZE];
	char tupleStr[BUF_SIZE];
	char outputStr[BUF_SIZE];
	char users[numBufs][5];
	int userNum = 0;
	int userIdx = 0;
	bool userExist;
	char delim[] = ",";
	char *token;
	char *rest = NULL;
	struct tuple_s tuple;

	// Get array of tupleBuffers as input to the thread
	//struct tupleBuffer_s *bufferStructs = (struct tupleBuffer_s *) arg;

	// Set initial users array to be all nulls
	for (i = 0; i < numBufs; i++)
		users[i][0] = '\0';

	// Loop through input until empty
	while (fgets(inputBuf, INPUT_STR_LEN + 1, stdin) != NULL)
	{
		inputBuf[INPUT_STR_LEN - 1] = '\0';
		// Parse the string into the tupleArray
		stringFormat(inputBuf, tupleStr);
		rest = tupleStr;
		token = strtok_r(rest, delim, &rest);
		strcpy(tuple.userID, token);
		token = strtok_r(NULL, delim, &rest);
		strcpy(tuple.action, token);
		token = strtok_r(NULL, delim, &rest);
		strcpy(tuple.topic, token);

		// Check if we need to add an ID to the users array
		userExist = false;
		for (i = 0; i < numBufs; i++)
		{
			if (!strcmp(tuple.userID, users[i]))
			{
				userExist = true;
				userIdx = i;
			}
		}
		// If not found, then increment userNum and copy into users array
		if (userExist == false && userNum < numBufs)
		{
			userIdx = userNum;
			strcpy(users[userNum++], tuple.userID);
			userExist = true;
		}

		if (userExist == true)
		{
			// Process the scores of a tuple
			switch (tuple.action[0])
			{
				case 'P':
					tuple.score = P_WEIGHT;
					break;
				case 'L':
					tuple.score = L_WEIGHT;
					break;
				case 'D':
					tuple.score = D_WEIGHT;
					break;
				case 'C':
					tuple.score = C_WEIGHT;
					break;
				case 'S':
					tuple.score = S_WEIGHT;
					break;
				default:
					tuple.score = 0;
					break;
			}

			// Print the output in the desired format to stdout
			sprintf(outputStr, "(%s,%s,%d)", tuple.userID, tuple.topic, tuple.score);

			// Once data is in its respective categories and in string form,
			// we need to check if we can write it out to the reducer thread
			// So we attempt to acquire the mutex lock
			threadErr = pthread_mutex_lock(&(bufferStructs[userIdx].mtx));
			errorHandler(threadErr);

			// If the lock is acquired but the buffer is empty, wait until awoken
			while (bufferStructs[userIdx].lastIdx == (bufSlots - 1))
			{
				threadErr = pthread_cond_wait(&(bufferStructs[userIdx].isFull), &(bufferStructs[userIdx].mtx));
				errorHandler(threadErr);
			}

			// Once the mutex is acquired and the desired buffer to write to is not full...
			// Place the outputStr in the next spot and increment lastIdx
			strcpy(bufferStructs[userIdx].tupleBuf[++(bufferStructs[userIdx].lastIdx)], outputStr);

			// Release the lock
			threadErr = pthread_mutex_unlock(&(bufferStructs[userIdx].mtx));
			errorHandler(threadErr);

			// Awake the sleeping consumer
			threadErr = pthread_cond_signal(&(bufferStructs[userIdx].isEmpty));
			errorHandler(threadErr);
		}
	}

	// Send conditional signals to every consumer before exiting
	for (i = 0; i < numBufs; i++)
	{
		threadErr = pthread_mutex_lock(&(bufferStructs[i].mtx));
		errorHandler(threadErr);
	}
	// Once no more input to the mapper, notify reducer threads
	*prodDone = true;
	// Send conditional signals to every consumer before exiting
	for (i = 0; i < numBufs; i++)
	{
		threadErr = pthread_mutex_unlock(&(bufferStructs[i].mtx));
		errorHandler(threadErr);
	}

	// Send conditional signals to every consumer before exiting
	for (i = 0; i < numBufs; i++)
	{
		threadErr = pthread_cond_signal(&(bufferStructs[i].isEmpty));
		errorHandler(threadErr);
	}

	// Exit safely
	return;
}

// Define the reducer thread function here
void reducerFunc(struct tupleBuffer_s *bufferStruct)
{
	// Declare necessary variables
	//struct tupleBuffer_s *bufferStruct = (struct tupleBuffer_s *) arg;
	int i, threadErr, found;
	char inputBuf[BUF_SIZE];
	char tupleStr[BUF_SIZE];
	char *token;
	char *rest = NULL;
	char delim[] = ",";
	struct tuple_s totals[MAX_TOPICS];
	struct tuple_s tempTuple;
	int topicNum = 0;

	// While the producer thread has not finished
	for (;;)
	{
		// Attempt to acquire the mutex lock
		threadErr = pthread_mutex_lock(&(bufferStruct->mtx));
		errorHandler(threadErr);

		// If the lock is acquired but the buffer is empty, wait until awoken
		while (bufferStruct->lastIdx == -1)
		{
			// Only break out and do the end stuff here when prodDone is done
			// AND buffer is empty
			if (*prodDone)
			{
				// Final printing of output goes here
				for (i = 0; i < topicNum; i++)
					fprintf(stdout, "(%s,%s,%d)\n", totals[i].userID, totals[i].topic, totals[i].score);

				// Set lastIdx to -2 so that main loop will break
				bufferStruct->lastIdx = -2;
			}
			else
			{
				// Only wait if the buffer is empty AND the producer is NOT done
				threadErr = pthread_cond_wait(&(bufferStruct->isEmpty), &(bufferStruct->mtx));
				errorHandler(threadErr);
			}
		}

		// Conditional to break the loop when done
		if (bufferStruct->lastIdx == -2)
		{
			// Make sure to unlock the mutex
			threadErr = pthread_mutex_unlock(&(bufferStruct->mtx));
			errorHandler(threadErr);
			
			// Then break out of the for loop of consumption	
			break;
		}


		// ***HERE IS WHERE THE ACTUAL CONSUMING PORTION IS DONE***
		// Remove the last entry in the tuple buffer
		strcpy(inputBuf, bufferStruct->tupleBuf[(bufferStruct->lastIdx)--]);

		// Parse the string into the tuple array of totals
		stringFormat(inputBuf, tupleStr);
		rest = tupleStr;
		token = strtok_r(rest, delim, &rest);
		strcpy(tempTuple.userID, token);
		token = strtok_r(NULL, delim, &rest);
		strcpy(tempTuple.topic, token);
		token = strtok_r(NULL, delim, &rest);
		tempTuple.score = atoi(token);

		found = 0;
		// Loop through existing tuples for matching topics
		for (i = 0; i < topicNum; i++)
		{
			// If the topics are the same, add the scores up
			if (!strcmp(totals[i].topic, tempTuple.topic))
			{
				totals[i].score += tempTuple.score;
				found = 1;
			}
		}

		// if not found, add to the tuple array of totals
		if (found == 0)
		{
			strcpy(totals[topicNum].userID, tempTuple.userID);
			strcpy(totals[topicNum].topic, tempTuple.topic);
			totals[topicNum].score = tempTuple.score;
			topicNum++;
		}

		// Release the lock
		threadErr = pthread_mutex_unlock(&(bufferStruct->mtx));
		errorHandler(threadErr);

		// Wake the sleeping producer if necessary
		threadErr = pthread_cond_signal(&(bufferStruct->isFull));
		errorHandler(threadErr);
	}

	// Exit safely
	exit(0);
}


// Main function
int main(int argc, char *argv[])
{
	// Declare and initialize necessary variables
	int i, j, threadErr;
	prodDone = (bool *)mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*prodDone = false;
	bufSlots = atoi(argv[1]);
	numBufs = atoi(argv[2]);
	pthread_mutexattr_t attrmutex;
	pthread_condattr_t attrcond;

	// Initialize pshared for mutex and conditional attributes
	pthread_mutexattr_init(&attrmutex);
	pthread_condattr_init(&attrcond);
	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
	pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED);

	// Create pointer for reducers array to be memory mapped
	struct tupleBuffer_s *reducers;

    reducers = (struct tupleBuffer_s *)mmap(NULL, sizeof(struct tupleBuffer_s) * numBufs, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (reducers == MAP_FAILED)
        exit(1);

	// Loop through and allocate and initialize values for tupleBuffer structures
	for (i = 0; i < numBufs; i++)
	{
		// Loop through and create buffers for tuples
		reducers[i].tupleBuf = (char **)mmap(NULL, sizeof(char *) * bufSlots, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		for (j = 0; j < bufSlots; j++)
			reducers[i].tupleBuf[j] = (char *)mmap(NULL, sizeof(char) * TUPLE_STRING, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

		reducers[i].lastIdx = -1;

		// Initialize the two conditional signals
		threadErr = pthread_cond_init(&(reducers[i].isFull), &attrcond);
		errorHandler(threadErr);

		threadErr = pthread_cond_init(&(reducers[i].isEmpty), &attrcond);
		errorHandler(threadErr);

		// Initialize the mutex lock
		threadErr = pthread_mutex_init(&(reducers[i].mtx), &attrmutex);
		errorHandler(threadErr);
	}

	// Fork as necessary for numBufs number of reducers
	for (i = 0; i < numBufs; i++)
	{
		// Fork and run each reducer function but don't exit until done
		if (fork() == 0)	
			reducerFunc(&(reducers[i]));
	}

	// Run the mapper function
	mapperFunc(reducers);

	// Wait for the reducer processes to return
	for (i = 0; i < numBufs; i++)
		wait(NULL);

	// Clean up the mutexes
	for (i = 0; i <= numBufs; i++)
	{
		pthread_mutex_destroy(&(reducers[i].mtx));
		pthread_cond_destroy(&(reducers[i].isFull));
		pthread_cond_destroy(&(reducers[i].isEmpty));
	}
	pthread_mutexattr_destroy(&attrmutex);
	pthread_condattr_destroy(&attrcond);

	return 0;
}


// Define helper functions

// Used to remove parentheses and white space from a string
static void stringFormat(char *inputStr, char *outputStr)
{
	int i, j = 0;
	for (i = 0; i < strlen(inputStr); i++)
	{
		if (inputStr[i] != '(' && inputStr[i] != ')')
			outputStr[j++] = inputStr[i];
	}
	outputStr[j] = '\0';
}

static void errorHandler(int threadErr)
{
	if (threadErr != 0)
		{
			printf("Error in a pthreads function.\n");
			exit(1);
		}
}


