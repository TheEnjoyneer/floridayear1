/* mapper.c
 * Christopher Brant
 * Programming Assignment 1
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 1/23/20
 */

#include <stdio.h>
#include <stdlib.h>
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
struct tuple {
	int userID;
	char action;
	char topic[TOPIC_LENGTH];
	int score;
};

// Declare helper functions




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
	int count, tupleCount, i;
	for (c = getc(inputFile); c != EOF; c = getc(inputFile))
		count++;
	tupleCount = count / INPUT_STR_LEN;
	fseek(inputFile, 0, SEEK_SET);

	// Declare Array of Structs
	char inputBuf[INPUT_STR_LEN + 1];
	struct tuple tupleArray[count];
	for (i = 0; i < count; i++)
	{
		// Read the data into the buffer
		fread(&inputBuf, sizeof(char), INPUT_STR_LEN, inputFile);
		inputBuf[INPUT_STR_LEN] = "\0";
		printf(inputBuf[i]);


	}

	return 0;
}







// Define helper functions





