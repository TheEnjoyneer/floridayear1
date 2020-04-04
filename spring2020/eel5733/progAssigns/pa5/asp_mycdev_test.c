/* HelloDriver.c
 * Christopher Brant
 * EEL 5733 Advanced Systems Programming
 * Device Driver In-Class Activity
 * 3/18/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char data[128];
	int fd = open("/dev/tux0", O_RDWR);

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
