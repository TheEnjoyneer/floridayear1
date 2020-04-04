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
#include <pthreads.h>

int main(void)
{
	char data[128];
	int fd[3];

	fd[0] = open("/dev/mycdev0", O_RDWR);
	fd[1] = open("/dev/mycdev1", O_RDWR);
	fd[2] = open("/dev/mycdev2", O_RDWR);

	if (fd < 0)
	{
		printf("Error in opening device file.\n");
		return 1;
	}
	
	if (fork() == 0)
	{
		// Write to the device
		write(fd, "\nChristopher Brant says: Hello tuxdrv!\n", 39);
		exit(0);
	}
	else
	{

		// Possibly read from the driver here
		read(fd, data, 39);
		data[39] = '\0';
		
		// Print the value that was read
		printf("Data read from the device driver: %s", data);
	}

	// Close the file descriptor
	close(fd);

	return 0;
}
