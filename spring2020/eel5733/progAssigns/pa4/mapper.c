/* mapper.c
 * Christopher Brant
 * Programming Assignment 1
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 1/23/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Define the rules for weights
#define P_WEIGHT 50
#define L_WEIGHT 20
#define D_WEIGHT -10
#define C_WEIGHT 30
#define S_WEIGHT 40
#define TOPIC_LENGTH 16
#define INPUT_STR_LEN 25

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
int main(int argc, char *argv[])
{
	// Attempt to open and verify opening of file to read
	FILE *inputFile;
	inputFile = fopen(argv[1], "r");
	if (inputFile == NULL)
	{
		fprintf(stderr, "Error opening file to read from.\n");
		exit(1);
	}

	char c;
	int i, tupleCount = 0;
	while ((c = getc(inputFile)) != EOF)
		if (c == ')')
			tupleCount++;
	// Reset the file cursor
	fseek(inputFile, 0, SEEK_SET);

	// Declare Array of Structs
	char inputBuf[INPUT_STR_LEN];
	char tupleStr[INPUT_STR_LEN];
	struct tuple_s tuple;
	for (i = 0; i < tupleCount; i++)
	{
		// Read the data into the buffer
		fread(&inputBuf, sizeof(char), INPUT_STR_LEN - 1, inputFile);
		fgetc(inputFile);
		inputBuf[INPUT_STR_LEN - 1] = '\0';

		// Parse the string into the tupleArray
		stringFormat(inputBuf, tupleStr);

		char delim[] = ",";
		char *token;
		token = strtok(tupleStr, delim);
		strcpy(tuple.userID, token);
		token = strtok(NULL, delim);
		strcpy(tuple.action, token);
		token = strtok(NULL, delim);
		strcpy(tuple.topic, token);

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
		fprintf(stdout, "(%s,%s,%d)\n", tuple.userID, tuple.topic, tuple.score);
	}

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






