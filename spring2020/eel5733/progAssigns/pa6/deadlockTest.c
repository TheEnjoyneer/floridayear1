/* deadlockTest.c
 * Christopher Brant
 * Programming Assignment 6
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 4/15/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)
#define DEVNAME "/dev/assignment6"


// Declare helper function prototypes
void operations(int fd, char ch);


// Main func
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i, fd[2];
	char ch;
	int deadlockScenario;

	// Quick check for args
	if (argc < 3)
	{
		fprintf(stderr, "Deadlock scenario number not specified\n");
		fprintf(stderr, "Correct usage: ./deadlockTest <deadlock scenario #> <device file name>\n");
		return 1;
	}

	// Set deadlockScenario now that we checked args
	deadlockScenario = atoi(argv[1]);

	// Switch statement for the scenarios to play out
	switch(deadlockScenario)
	{
		// Run procedure to test the scenario given in the readMe as deadlock scenario 1
		case 1:

			/* This will be tested by forking 2 processes that both attempt to open the driver, one will
			  sleep immediately so the other will be able to open the driver first (P1), then that process
			  will sleep so that the other will be able to attempt to open and be waiting for devc->sem2
			  which it cannot acquire (P2), then P1 will attempt to switch from MODE1 to MODE2 via e2_ioctl,
			  and at that time, a deadlock SHOULD occur. */

			if (fork() == 0)
			{
				fd[0] = open(argv[2], O_RDWR);
				sleep(2);
				operations(fd[0], 'p');
			}
			else
			{
				sleep(1);
				fd[1] = open(argv[2], O_RDWR);
				sleep(3);
			}

			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 2
		case 2:

			/* This will be tested similarly to Scenario 1, as two processes will be forked off and
			  the first process will open the device, but it will then immediately switch to MODE2
			  while the second process sleeps for a moment, and then the second process will open the
			  device and it will open in MODE2.  Then both processes will attempt to switch to MODE1
			  at the same time, and with devc->count2 being more than 0, and having either process
			  not being able to release and decrement devc->count2, a deadlock SHOULD occur. */

			if (fork() == 0)
			{
				fd[0] = open(argv[2], O_RDWR);
				operations(fd[0], 'p');
				sleep(2);
				operations(fd[0], 's');
			}
			else
			{
				sleep(1);
				fd[1] = open(argv[2], O_RDWR);
				operations(fd[1], 's');
			}

			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 3
		case 3:

			/* This will be tested basically the same as the scenario was given, so sleep statements
			  will be inserted in the first process to open after it has switched to MODE2 so that
			  second process to open will open in MODE2, and it is able to able to attempt to switch
			  to MODE1 first, then the first process will close the device. */

			if (fork() == 0)
			{
				fd[0] = open(argv[2], O_RDWR);
				operations(fd[0], 'p');
				sleep(2);
				close(fd[0]);
			}
			else
			{
				sleep(1);
				fd[1] = open(argv[2], O_RDWR);
				operations(fd[1], 's');
			}

			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 4
		case 4:

			/* This will be as simple as it sounds, just attempt to open the device twice in the same process. */

			fd[0] = open(argv[2], O_RDWR);
			fd[1] = open(argv[2], O_RDWR);

			break;

		// Default case for if the given input is not valid
		default:
			fprintf(stderr, "\nError: Invalid input option for Deadlock Scenario.\n");
			break;
	}

	return 0;
}


// Define helper funcs
void operations(int fd, char ch)
{
	// Declare necessary variables
	int rc;
	char write_buf[100], read_buf[100];
	
	// Switch statement for running the different VFS operations
	switch(ch)
	{
	// case for writing data to the device..."w" for "write" *duh*
	case 'w':
		printf("Enter Data to write: ");
		scanf(" %[^\n]", write_buf);
		write(fd, write_buf, sizeof(write_buf));
		break;

	// case for switching to mode 1..."s" for "singular"
	case 's':
		printf("\n Will clear data \n");
		rc = ioctl(fd, E2_IOCMODE1, 0);
		if (rc == -1)
		{ 
			perror("\n***error in ioctl***\n");
			return -1;
		}
		printf("\n In mode 1 now \n");
		break;

	// case for switching to mode2..."p" for "plural"
	case 'p':
		printf("\n Will switch to mode2 \n");
		rc = ioctl(fd, E2_IOCMODE2, 0);
		if (rc == -1)
		{ 
			perror("\n***error in ioctl***\n");
			return -1;
		}
		printf("\n In mode 2 now \n");
		break;

	// WILL NEED TO CHANGE THIS SECTION TO FIX IT CORRECTLY
	// case for reading data from the device..."r" for "read" *duh*
	case 'r':
		if (read(fd, read_buf, sizeof(read_buf)) > 0)
		{
			printf("\ndevice: %s\n", read_buf);
		}
		else
		{
			fprintf(stderr, "Reading failed\n");
		}
		break;

	default:
		printf("Command not recognized\n");
		break;
	}
}