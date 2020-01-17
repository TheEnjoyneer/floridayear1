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
	char userID[5];
	char action[2];
	char topic[TOPIC_LENGTH];
	int score;
};

// Declare helper functions
void removeParentheses(char *inputStr, char *outputStr);



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
	int tupleCount, i;
	int count = 0;
	for (c = getc(inputFile); c != EOF; c = getc(inputFile))
		count++;
	tupleCount = count / INPUT_STR_LEN;
	fseek(inputFile, 0, SEEK_SET);

	// Declare Array of Structs
	char inputBuf[INPUT_STR_LEN];
	char tupleStr[INPUT_STR_LEN];
	struct tuple tupleArray[tupleCount];
	for (i = 0; i < tupleCount; i++)
	{
		// Read the data into the buffer
		fread(&inputBuf, sizeof(char), INPUT_STR_LEN - 1, inputFile);
		fgetc(inputFile);
		inputBuf[INPUT_STR_LEN - 1] = '\0';
		// int j;
		// for (j = 0; j < INPUT_STR_LEN; j++)
		// 	printf("%c", inputBuf[j]);
		// printf("\n");

		// Allocate memory for struct


		// Parse the string into the tupleArray
		removeParentheses(inputBuf, tupleStr);

		char delim[] = ",";
		char *token;
		token = strtok(tupleStr, delim);
		strcpy(tupleArray[i].userID, token);
		token = strtok(NULL, delim);
		strcpy(tupleArray[i].action, token);
		token = strtok(NULL, delim);
		strcpy(tupleArray[i].topic, token);

		printf("userID: %s  ", tupleArray[i].userID);
		printf("action: %s  ", tupleArray[i].action);
		printf("topic: %s  \n", tupleArray[i].topic);





	}

	return 0;
}







// Define helper functions
void removeParentheses(char *inputStr, char *outputStr)
{
	int i, j = 0;
	for (i = 0; i < strlen(inputStr); i++)
	{
		if (inputStr[i] != '(' && inputStr[i] != ')')
			outputStr[j++] = inputStr[i];
	}
	outputStr[j] = '\0';
}





