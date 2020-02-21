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

// Declare static variables
static bool noMoreTransfers = false;
static sem_t mtx;
static sem_t *threadsLock;
static struct bankAccount *accounts;
static bool *accountStates;
static int numWorkers;

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
	struct transferOrder *currOrder;
};


// Declare Helper functions here
void getAccounts(int workerNum, int fromIdx, int toIdx);
void putAccounts(int workerNum, int fromIdx, int toIdx);
void transferFunds(int fromIdx, int toIdx, int amount);
int right(int workerNum);
int left(int workerNum);


// Declare thread functions here
static void *workerThread(void *arg)
{
	// Declare necessary variables
	//int i, j;
	int fromIdx, toIdx, moveAmount, cont = 1;
	struct workerParams *workOrder = (struct workerParams *) arg;
	struct transferOrder *transferVals;
	int threadNum = workOrder->workerID;


	printf("Hello from a worker thread.\n");

	// Loop through and wait or complete transfers until theres no more transfers.
	while (cont)
	{
		sem_wait(&mtx);
		if (workOrder->currOrder != NULL)
		{
			// Reset loop vals
			transferVals = workOrder->currOrder;

			// COME BACK AND MAKE SURE THIS IS CORRECT
			fromIdx = transferVals->fromAccNum;
			toIdx = transferVals->toAccNum;
			moveAmount = transferVals->amount;

			printf("This worker thread has transfer %p.\n", workOrder->currOrder);
			printf("Worker #%d has a job to do...\n\n", workOrder->workerID);

			// Attempt to get the account locks
			getAccounts(threadNum, fromIdx, toIdx);

			// Print test statement
			printf("Transferring %d from Account %d to Account %d\n", transferVals->amount, transferVals->fromAccNum, transferVals->toAccNum);

			// Do the transfering
			transferFunds(fromIdx, toIdx, moveAmount);

			// Put back/unlock the accounts when done
			putAccounts(threadNum, fromIdx, toIdx);

			// Set the currOrder to be null now
			workOrder->currOrder = NULL;

			// Unlock the mutex
			sem_post(&mtx);
		}
		else
		{
			// If no more transfers break the while loop
			if (noMoreTransfers)
				cont = 0;
			// Release the lock in the case that it doesn't have work to do
			sem_post(&mtx);
		}
	}

	// Exit thread safely
	pthread_exit(NULL);
}


// Main function here
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i, j, ordered, threadErr;
	struct transferOrder *transfList;
	struct workerParams *workerOrders;
	numWorkers = atoi(argv[2]);

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
	accountStates = (bool *)malloc(sizeof(bool) * numWorkers);

	// Initialize all worker params
	for (i = 0; i < numWorkers; i++)
	{
		workerOrders[i].workerID = i;
		workerOrders[i].currOrder = NULL;
	}

	// Initialize all account states
	for (i = 0; i < accountCount; i++)
		accountStates[i] = false;


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
	sem_init(&mtx, 0, 1);
	threadsLock = (sem_t *)malloc(sizeof(sem_t) * numWorkers);
	for (i = 0; i < numWorkers; i++)
		sem_init(&(threadsLock[i]), 0, 0);

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
		ordered = 0;
		// Lock semaphore
		sem_wait(&mtx);
		for (j = 0; j < numWorkers; j++)
		{
			if (workerOrders[j].currOrder == NULL)
			{
				// Set new current order
				workerOrders[j].currOrder = &(transfList[i]);
				ordered = 1;
				printf("Assigning Worker %d transfer %d. Which is pointer %p.\n", j, i, &(transfList[i]));
				// Break out of inner for loop
				break;
			}
		}
		// Make loop go again at this transfList order value if this is the case
		if (ordered == 0)
			i--;

		// Unlock semaphore
		sem_post(&mtx);
	}

	// Lock workerLock semaphore and set noMoreTransfers to be true
	// MAKE SURE THIS IS DONE CORRECTLY AS FOR NOW IT PROBABLY ISN'T SINCE
	// THIS WON'T TAKE CARE OF EVERYTHING SIGNAL-WISE
	sem_wait(&mtx);
	noMoreTransfers = true;
	sem_post(&mtx);
	sem_post(&mtx);
	sem_post(&mtx);
	sem_post(&mtx);

	printf("No more transfers.\n");

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

	for (i = 0; i < accountCount; i++)
		printf("Account %d has a balance of %d.\n", accounts[i].accNum, accounts[i].balance);

	// Free all allocated memory
	free(accountStates);
	free(threadsLock);
	free(workerOrders);
	free(transfList);
	free(accounts);

	return 0;
}


// Define Helper functions here

// getAccounts is essentially the get_forks(i) function in my implementation
void getAccounts(int workerNum, int fromIdx, int toIdx)
{
	while (accountStates[fromIdx] == false || accountStates[toIdx] == false)
	{
		sem_post(&mtx);
		sem_wait(&(threadsLock[workerNum]));
		sem_wait(&mtx);
	}

	// Set to be false after successfully leaving the while loop
	accountStates[fromIdx] = false;
	accountStates[toIdx] = false;

	// Unlock mutex
	sem_post(&mtx);
}

// putAccounts is essentially the put_forks(i) function in my implementation
void putAccounts(int workerNum, int fromIdx, int toIdx)
{
	// Lock the mutex
	sem_wait(&mtx);

	// Change account states values
	accountStates[fromIdx] = true;
	accountStates[toIdx] = true;

	// Post semaphore updates
	sem_post(&(threadsLock[left(workerNum)]));
	sem_post(&(threadsLock[right(workerNum)]));
}

// transferFunds just takes the two account numbers and does the balance transfer
void transferFunds(int fromIdx, int toIdx, int amount)
{
	// Perform the balance transfer
	accounts[fromIdx].balance -= amount;
	accounts[toIdx].balance += amount;
}

int right(int workerNum) { return (workerNum + 1) % numWorkers; }

int left(int workerNum) { return (workerNum + numWorkers - 1) % numWorkers; }















