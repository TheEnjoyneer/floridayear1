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
#include <string.h>

// Define all pound define variables
#define P_WEIGHT 50
#define L_WEIGHT 20
#define D_WEIGHT -10
#define C_WEIGHT 30
#define S_WEIGHT 40
#define TOPIC_LENGTH 16
#define INPUT_STR_LEN 25
#define TUPLE_STRING 27
// May not need those below this comment
#define MAX_TOPICS 10
#define BUF_SIZE 30


// Declare static variables
static int bufSlots;
static int numBufs;
static bool prodDone = false;


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
	pthread_cond_t full;	// Condition for full or available buffer
};

// Define the mapper thread function here
void *mapperThread(void *arg)
{
	// Declare necessary variables
	int i;
	char inputBuf[BUF_SIZE];
	char tupleStr[BUF_SIZE];
	char outputStr[BUF_SIZE];
	char users[numBufs][5];
	int userNum = 0;
	int userIdx = 0;
	bool userExist;
	char delim[] = ",";
	char *token;
	struct tuple_s tuple;

	// Get array of tupleBuffers as input to the thread
	struct tupleBuffer_s *bufferStructs = (struct tupleBuffers_s *) arg;

	// Set initial users array to be all nulls
	for (i = 0; i < numBufs; i++)
		users[i][0] = '\0';

	// Loop through input until empty
	while (fgets(inputBuf, BUF_SIZE, stdin) != NULL)
	{
		// Parse the string into the tupleArray
		stringFormat(inputBuf, tupleStr);
		token = strtok(tupleStr, delim);
		strcpy(tuple.userID, token);
		token = strtok(NULL, delim);
		strcpy(tuple.action, token);
		token = strtok(NULL, delim);
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
		if (userExist == false && userNum <= numBufs)
		{
			userIdx = userNum;
			strcpy(users[userNum++], tuple.userID);
		}

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
		sprintf(outputStr, "(%s,%s,%d)\n", tuple.userID, tuple.topic, tuple.score);

		// Once data is in its respective categories and in string form,
		// we need to check if we can write it out to the reducer thread

		/*
			HERE IS WHERE THE MUTEX TO WRITE TO THE BUFFER WILL GO
			INCLUDING THE WAITING WHEN THERES A FULL BUFFER
		*/



	}

	// Once no more input to the mapper, notify reducer threads
	prodDone = true;

	// Exit safely
	pthread_exit(NULL);
}

// Define the reducer thread function here
void *reducerThread(void *arg)
{
	// Declare necessary variables
	struct tupleBuffer_s bufferStruct = *((struct tupleBuffers_s *) arg);

	/*
		MUTEX STUFF FOR READING AND CONSUMING VALUES HERE

		WAIT FOR THERE TO BE VALUES TO CONSUME

		CONSUME VALUES AND SAVE ALL DATA


		WHEN prodDone IS TRUE, THEN PRINT ALL DATA TO THE SCREEN

		THEN EXIT THE THREAD
	*/





	// Exit safely
	pthread_exit(NULL);
}

// Declare helper functions
void stringFormat(char *inputStr, char *outputStr);


// Main function
int main(int argc, char *argv[])
{
	// Declare and initialize necessary variables
	int i, j;
	bufSlots = atoi(argv[1]);
	numBufs = atoi(argv[2]);

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
		pthread_create(&threads[i], NULL, reducerThread, &(reducers[i]));
	// Make the producer thread
	pthread_create(&threads[numBufs], NULL, mapperThread, &reducers);





	// What here? All the rest of the threads are running...
	// Is there a need to use thread_join here?






	return 0;
}


// Define helper functions

// Used to remove parentheses and white space from a string
void stringFormat(char *inputStr, char *outputStr)
{
	int i, j = 0;
	for (i = 0; i < strlen(inputStr); i++)
	{
		if (inputStr[i] != '(' && inputStr[i] != ')')
			outputStr[j++] = inputStr[i];
	}
	outputStr[j] = '\0';
}


