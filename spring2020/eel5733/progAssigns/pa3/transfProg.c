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
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXLINE 50
#define FREE 0
#define WAITING 1
#define PENDING 2

// Declare static variables
static bool noMoreTransfers = false;
static sem_t workerLock;
static sem_t *accLock;

// Define structures here
struct transferOrder {
	int fromAccNum;
	int toAccNum;
	int amount;
};

struct bankAccount {
	int accNum;
	int balance;
};

struct workerParams {
	int workerID;
	int state;
	struct transferOrder *currOrder;
};


// Declare thread functions here
static void *workerThread(void *arg)
{
	// Read the 






	// Exit thread safely
	pthread_exit(NULL);
}


// Declare Helper functions here
void getAccounts(int workerNum, int toAcc, int fromAcc);
void testAccounts(int workerNum, int toAcc, int fromAcc);
void putAccounts(int workerNum, int toAcc, int from Acc);


// Main function here
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i, j, ordered, threadErr;
	int numWorkers = atoi(argv[2]);
	int assignToWorker = 0;
	struct bankAccount *accounts;
	struct transferOrder *transfList;
	struct transferOrder *workerOrders;

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
		if (inputBuf[0] == 'T')
			transfCount++;
		else
			accountCount++;
	}

	// Reset the file pointer cursor and then malloc for arrays
	fseek(inputFile, 0, SEEK_SET);

	accounts = (struct bankAccount *)malloc(sizeof(struct bankAccount) * accountCount);
	transfList = (struct transferOrder *)malloc(sizeof(struct transferOrder) * transfCount);
	workerOrders = (struct workerParams *)malloc(sizeof(struct workerParams) * numWorkers);

	// Initialize all worker params
	for (i = 0; i < numWorkers; i++)
	{
		workerOrders[i].workerID = i;
		workerOrders[i].state = FREE;
		workerOrders[i].currOrder = NULL;
	}


	while (fgets(inputBuf, MAXLINE, inputFile) != NULL)
	{
		if (inputBuf[0] == 'T')
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
			accounts[accountNum].balance = atoi(token);
		}
	}

	// After all input has been parsed close the input file
	fclose(inputFile);



	// // TESTING PURPOSES ONLY
	// for (i = 0; i < accountCount; i++)
	// 	printf("Account Number: %d has Initial Balance: %d\n", accounts[i].accNum, accounts[i].balance);

	// printf("Transfer List includes the following transfers:\n");
	// for (i = 0; i < transfCount; i++)
	// 	printf("From: %d, To: %d, Amount: %d\n", transfList[i].fromAccNum, transfList[i].toAccNum, transfList[i].amount);



	// Vars for threads and semaphores
	pthread_t workers[numWorkers];

	// Initial setup for threads and semaphores before creating threads
	sem_init(&workerLock, 0, 1);
	accountLock = (sem_t *)malloc(sizeof(sem_t) * accountCount);
	for (i = 0; i < accountcount; i++)
		sem_init(&(accLock[i]), 0, 0);

	// Now to create the worker threads
	for (i = 0; i < numWorkers; i++)
	{
		threadErr = pthread_create(&workers[i], NULL, workerThread, &(workerOrders[i]));
		if (threadErr != 0)
		{
			printf("Error in pthread_mutex_init function.\n");
			exit(1);
		}
	}

	// Now assign transfer orders to threads
	for (i = 0; i < transfCount; i++)
	{


	}

	// Setup threads to join 
	for (i = 0; i < numWorkers; i++)
	{
		threadErr = pthread_join(workers[i], NULL);
		if (threadErr != 0)
		{
			printf("Error in pthread_join function.\n");
			exit(1);
		}
	}

	// Free all allocated memory
	free(accountLock);
	free(workerOrders);
	free(transfList);
	free(accounts);

	return 0;
}


// Define Helper functions here

// getAccounts is essentially the get_forks(i) function in my implementation
void getAccounts(int workerNum, int toAcc, int fromAcc)
{

}

// testAccounts is essentially the test(i) function in my implementation
void testAccounts(int workerNum, int toAcc, int fromAcc)
{

}

// putAccounts is essentially the put_forks(i) function in my implementation
void putAccounts(int workerNum, int toAcc, int from Acc)
{

}















