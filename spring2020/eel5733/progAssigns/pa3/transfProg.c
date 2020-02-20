/* transfProg.c
 * Christopher Brant
 * Programming Assignment 3
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 2/22/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAXLINE 50

// Define structures here
struct transferOrder {
	int fromAccNum;
	int toAccNum;
	int amount;
};

struct bankAccount {
	int accNum;
	int balance;
}


// Declare thread functions here
static void *workerThread(void *arg)
{







	// Exit thread safely
	pthread_exit(NULL);
}


// Declare Helper functions here


// Main function here
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i;
	int numWorkers = atoi(argv[2]);
	int assignToWorker = 0;
	struct bankAccount *accounts;
	struct transferOrder *transfList;

	// Vars for account and transfer input
	int accountCount = 0;
	int transfCount = 0;
	int accountNum = 0;
	int transfNum = 0;

	// Vars for string splitting
	char inputBuf[MAXLINE];
	char delim[] = " ";
	char *rest = NULL;
	char *token;

	// Open input file for reading
	FILE *inputFile;
	inputFile = fopen(argv[1], "r");

	// Count number of accounts before allocating mem for the array of them.
	while (fgets(inputBuf, MAXLINE, inputFile) != NULL)
	{
		if (inputBuf[0] == "T")
			transfCount++;
		else
			accountCount++;
	}

	// Reset the file pointer cursor and then malloc for arrays
	fseek(inputFile, 0, SEEK_SET);

	accounts = (struct bankAccount *)malloc(sizeof(struct bankAccount) * accountCount);
	transfList = (struct transferOrder *)malloc(sizeof(struct transferOrder) * transfCount);


	while (fgets(inputBuf, MAXLINE, inputFile) != NULL)
	{
		if (inputBuf[0] == "T")
		{
			// Parse the data out here
			rest = inputBuf;
			token = strtok_r(rest, delim, &rest);
			// Now that the word "Transfer" has been removed split ints to the transfer order
			token = strtok_r(NULL, delim, &rest);
			transfList[transfNum].fromAccNum = atoi(token);
			token = strtok_r(NULL, delim, &rest);
			transfList[transfNum].toAccNum = atoi(token);
			token = strtok_r(NULL, delim, &rest);
			transfList[transfNum++].amount = atoi(token);
		}
		else
		{
			// Parse the data out here
			rest = inputBuf;
			token = strtok_r(rest, delim, &rest);
			accounts[accountNum].accNum = atoi(token);
			token = strtok_r(NULL, delim, &rest);
			accounts[accountNum++].balance = atoi(token);
		}
	}

	// After all input has been parsed close the input file
	fclose(inputFile);

	// TESTING PURPOSES ONLY
	for (i = 0; i < accountCount; i++)
		printf("Account Number: %d has Initial Balance: %d\n", accounts[i].accNum, accounts[i].balance);

	printf("Transfer List includes the following transfers:\n");
	for (i = 0; i < transfCount; i++)
		printf("From: %d, To: %d, Amount: %d\n", transfList[i].fromAccNum, transfList[i].toAccNum, transfList[i].amount);
	

	// Now to create the worker threads





	return 0;
}


// Define Helper functions here













