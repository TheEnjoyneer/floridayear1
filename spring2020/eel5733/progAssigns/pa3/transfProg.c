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
static struct bankAccount *accounts;
static int *accountStates;

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


// Declare thread functions here
static void *workerThread(void *arg)
{
	// Declare necessary variables
	struct workerParams *workOrder = (struct workerParams *) arg;
	struct transferOrder *transferVals = workOrder->currOrder;

	// Loop through and wait or complete transfers until theres no more transfers.
	while (1)
	{
		sem_wait(&workerLock);
		if (transferVals != NULL)
		{
			// Attempt to get the account locks
			// this function sem_posts
			getAccounts(transferVals);

			// Print test statement
			printf("Transferring %d from Account %d to Account %d\n", transferVals->amount, transferVals->fromAccNum, transferVals->toAccNum);

			// Put back/unlock the accounts when done
			// this function sem_waits and sem_posts for workerLock
			putAccounts(transferVals);
		}
		else
		{
			// If no more transfers break the while loop
			if (noMoreTransfers)
				break;
			// Release the lock in the case that it doesn't have work to do
			sem_post(&workerLock);
		}
	}

	// Exit thread safely
	pthread_exit(NULL);
}


// Declare Helper functions here
void getAccounts(struct transferOrder *order);
void testAccounts(struct transferOrder *order);
void putAccounts(struct transferOrder *order);
void transferFunds(int fromIdx, int toIdx, int amount);


// Main function here
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i, j, ordered, threadErr;
	int numWorkers = atoi(argv[2]);
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
	workerStates = (int *)malloc(sizeof(int) * numWorkers);

	// Initialize all worker params
	for (i = 0; i < numWorkers; i++)
	{
		workerOrders[i].workerID = i;
		workerOrders[i].currOrder = NULL;
	}

	// Initialize all account states
	for (i = 0; i < accountCount; i++)
		accountStates[i] = FREE;


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
		ordered = 0;
		// Lock semaphore
		sem_wait(&workerLock);
		for (j = 0; j < numWorkers; j++)
		{
			if (workerOrders[j].currOrder == NULL)
			{
				// Set new current order
				workerOrders[j].currOrder = &(transfList[i]);
				ordered = 1;
				// Break out of inner for loop
				break;
			}
		}
		// Make loop go again at this transfList order value if this is the case
		if (ordered == 0)
			i--;

		// Unlock semaphore
		sem_post(&workerLock);
	}

	// Lock workerLock semaphore and set noMoreTransfers to be true
	// MAKE SURE THIS IS DONE CORRECTLY AS FOR NOW IT PROBABLY ISN'T SINCE
	// THIS WON'T TAKE CARE OF EVERYTHING SIGNAL-WISE
	sem_wait(&workerLock);
	noMoreTransfers = true;
	sem_post(&workerLock);

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
	free(workerStates);
	free(accountLock);
	free(workerOrders);
	free(transfList);
	free(accounts);

	return 0;
}


// Define Helper functions here

// getAccounts is essentially the get_forks(i) function in my implementation
void getAccounts(int workerNum, struct transferOrder *order)
{

}

// testAccounts is essentially the test(i) function in my implementation
void testAccounts(int workerNum, struct transferOrder *order)
{
	if (workerStates[workerNum] == WAITING)

}

// putAccounts is essentially the put_forks(i) function in my implementation
void putAccounts(int workerNum, struct transferOrder *order)
{

}

// transferFunds just takes the two account numbers and does the balance transfer
void transferFunds(int fromIdx, int toIdx, int amount)
{
	// Perform the balance transfer
	accounts[fromIdx].balance -= amount;
	accounts[toIdx].balance += amount;
}















