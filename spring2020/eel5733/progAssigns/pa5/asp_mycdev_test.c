/* asp_mycdev_test.c
 * Christopher Brant
 * EEL 5733 Advanced Systems Programming
 * Assignment 5 Test Program
 * 4/4/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define ASP_CLEAR_BUF _IO('k', 1)

int main(void)
{
	char data[128];
	int fd[3];

	fd[0] = open("/dev/mycdev0", O_RDWR);
	//fd[1] = open("/dev/mycdev1", O_RDWR);
	//fd[2] = open("/dev/mycdev2", O_RDWR);

	if (fd[0] < 0)// || fd[1] < 0 || fd[2] < 0)
	{
		printf("Error in opening one of the device files.\n");
		return 1;
	}
	
	if (fork() == 0)
	{
		sleep(2);
		// Clear the buffer with the ioctl function
		ioctl(fd[0], ASP_CLEAR_BUF, NULL);

		// Write to the device
		write(fd[0], "\n Christopher Brant says: Hello /dev/mycdev0!\n", 45);
		//sleep(10);

		// // Let the other process read, clear, and write then try to read and then lseek and write then the other will read
		lseek(fd[0], 0, SEEK_SET);
		read(fd[0], data, 46);
		data[46] = '\0';
		printf("Data read from the device driver: %s\n", data);

		// // lseek and try and read again
		// printf("Now trying lseek\n");
		// lseek(fd[0], 4, SEEK_SET);
		// read(fd[0], data, 10);
		// data[10] = '\0';
		// printf("Data read from the device driver: %s\n", data);
		exit(0);


	}
	else
	{
		// // Clear the buffer with the ioctl function
		//ioctl(fd[0], ASP_CLEAR_BUF, NULL);

		// // Write to the device
		// write(fd[0], "\n Christopher Brant says: Hello /dev/mycdev0!\n", 45);

		// Possibly read from the driver here
		//sleep(2);
		lseek(fd[0], 0, SEEK_SET);
		read(fd[0], data, 45);
		data[45] = '\0';
		
		// Print the value that was read
		printf("Data read from the device driver: %s\n", data);

		//sleep(2);

		lseek(fd[0], 0, SEEK_SET);

		// lseek(fd[0], 4, SEEK_SET);
		// read(fd[0], data, 10);
		// data[10] = '\0';
		// printf("Data read from the device driver: %s\n", data);
		//sleep(1);

		// printf("Clearing the device driver buffer with ioctl\n");

		// // Clear the buffer with the ioctl function
		// ioctl(fd[0], ASP_CLEAR_BUF, NULL);

		// // Write new value to buffer
		write(fd[0], "\nChristopher Brant says: CRIKEY /dev/mycdev0!\n", 46);

		// lseek(fd[0], 0, SEEK_SET);

		// read(fd[0], data, 46);
		// data[46] = '\0';
		// printf("Data read from the device driver: %s\n", data);

		// // Sleep some more
		wait(NULL);

	}

	// Close the file descriptor
	close(fd[0]);
	//close(fd[1]);
	//close(fd[2]);

	return 0;
}
