/* reducer.c
 * Christopher Brant
 * Programming Assignment 4
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 3/14/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Define constants
#define MAX_TOPICS 10
#define BUF_SIZE 30
#define TOPIC_LENGTH 16
#define TUPLE_STRING 27

// Declare struct of tuple
struct tuple_s {
	char userID[5];
	char action[2];
	char topic[TOPIC_LENGTH];
	int score;
};

// Declare helper functions
void stringFormat(char *inputStr, char *outputStr);

// Main function
int main(void)
{
	int i, found, tupleCount;
	char inputBuf[BUF_SIZE];
	char tupleStr[BUF_SIZE];
	size_t size = BUF_SIZE;
	char delim[] = ",";
	char *token;
	char currUserID[5];

	struct tuple_s tupleArray[MAX_TOPICS];
	struct tuple_s tempTuple;

	// Setup for looping
	fgets(inputBuf, BUF_SIZE, stdin);
	// Parse the string into the tupleArray
	stringFormat(inputBuf, tupleStr);
	token = strtok(tupleStr, delim);
	strcpy(tupleArray[0].userID, token);
	token = strtok(NULL, delim);
	strcpy(tupleArray[0].topic, token);
	token = strtok(NULL, delim);
	tupleArray[0].score = atoi(token);
	strcpy(currUserID, tupleArray[0].userID);
	tupleCount = 1;

	// continue looping here
	while (fgets(inputBuf, BUF_SIZE, stdin) != NULL)
	{
		// Parse the string into the tupleArray
		stringFormat(inputBuf, tupleStr);
		token = strtok(tupleStr, delim);
		strcpy(tempTuple.userID, token);
		token = strtok(NULL, delim);
		strcpy(tempTuple.topic, token);
		token = strtok(NULL, delim);
		tempTuple.score = atoi(token);

		if (strcmp(currUserID, tempTuple.userID))
		{
			// Print out all tuples with total scores
			for (i = 0; i < tupleCount; i++)
				fprintf(stdout, "(%s,%s,%d)\n", tupleArray[i].userID, tupleArray[i].topic, tupleArray[i].score);

			// Reset for next looping
			strcpy(currUserID, tempTuple.userID);
			strcpy(tupleArray[0].userID, tempTuple.userID);
			strcpy(tupleArray[0].topic, tempTuple.topic);
			tupleArray[0].score = tempTuple.score;
			tupleCount = 1;
		}
		else
		{
			found = 0;
			// Loop through existing tuples for matching topics
			for (i = 0; i < tupleCount; i++)
			{
				// If the topics are the same, add the scores up
				if (!strcmp(tupleArray[i].topic, tempTuple.topic))
				{
					tupleArray[i].score += tempTuple.score;
					found = 1;
				}
			}

			// if not found, add to the tuple array
			if (found == 0)
			{
				strcpy(tupleArray[tupleCount].userID, tempTuple.userID);
				strcpy(tupleArray[tupleCount].topic, tempTuple.topic);
				tupleArray[tupleCount].score = tempTuple.score;
				tupleCount++;
			}
		}
	}

	// Print out all tuples with total scores for the last user ID
	for (i = 0; i < tupleCount; i++)
		fprintf(stdout, "(%s,%s,%d)\n", tupleArray[i].userID, tupleArray[i].topic, tupleArray[i].score);
	
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




