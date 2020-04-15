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


// Declare helper function prototypes
void operations(int fd, char ch);


// Main func
int main(int argc, char *argv[])
{
	// Declare necessary variables
	int i;
	char ch;
	int deadlockScenario;

	// Quick check for args
	if (argc < 2)
	{
		fprintf(stderr, "Deadlock scenario number not specified\n");
		fprintf(stderr, "Correct usage: ./deadlockTest <deadlock scenario #>\n");
		return 1;
	}

	// Set deadlockScenario now that we checked args
	deadlockScenario = atoi(argv[1]);

	
	// Open the device file
	// int fd = open(dev_path, O_RDWR);
	// if(fd == -1)
	// {
	// 	printf("File %s either does not exist or has been locked by another "
	// 			"process\n", DEVICE);
	// 	exit(-1);
	// }
	

	switch(deadlockScenario)
	{
		// Run procedure to test the scenario given in the readMe as deadlock scenario 1
		case 1:


			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 2
		case 2:


			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 3
		case 3:


			break;

		// Run procedure to test the scenario given in the readMe as deadlock scenario 4
		case 4:


			break;

		// Default case for if the given input is not valid
		default:
			fprintf(stderr, "\nError: Invalid input option for Deadlock Scenario.\n");
			break;
	}



	// Probably will have to remove this close and do it in the case statement
	// close(fd);



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