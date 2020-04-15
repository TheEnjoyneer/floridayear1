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
	// Quick check for args
	if (argc < 2) {
		fprintf(stderr, "Deadlock scenario number not specified\n");
		fprintf(stderr, "Correct usage: ./deadlockTest <deadlock scenario #>\n");
		return 1;
	}

	// Declare necessary variables
	char dev_path[20];
	int i,fd;
	char ch, write_buf[100], read_buf[100];
	int offset, origin;
	sprintf(dev_path, "%s%d", DEVICE, dev_no);

	// Open the device file
	fd = open(dev_path, O_RDWR);
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
		exit(-1);
	}













	close(fd);
	return 0;
}


// Define helper funcs
void operations(int fd, char ch)
{
	switch(ch) {
	case 'w':
		printf("Enter Data to write: ");
		scanf(" %[^\n]", write_buf);
		write(fd, write_buf, sizeof(write_buf));
		break;

	case 's':
		printf("\n Will clear data \n");
		int rc = ioctl(fd, E2_IOCMODE1, 0);
		if (rc == -1) { 
			perror("\n***error in ioctl***\n");
			return -1;
		}
		printf("\n In mode 1 now \n");
		break;

	case 'p':
		printf("\n Will switch to mode2 \n");
		int rc = ioctl(fd, E2_IOCMODE2, 0);
		if (rc == -1) { 
			perror("\n***error in ioctl***\n");
			return -1;
		}
		printf("\n In mode 2 now \n");
		break;

	case 'r':
		printf("Origin \n 0 = beginning \n 1 = current \n 2 = end \n\n");
		printf(" enter origin :");
		scanf("%d", &origin);
		printf(" \n enter offset :");
		scanf("%d", &offset);
		lseek(fd, offset, origin);
		if (read(fd, read_buf, sizeof(read_buf)) > 0) {
			printf("\ndevice: %s\n", read_buf);
		} else {
			fprintf(stderr, "Reading failed\n");
		}
		break;

	default:
		printf("Command not recognized\n");
		break;

	}
}