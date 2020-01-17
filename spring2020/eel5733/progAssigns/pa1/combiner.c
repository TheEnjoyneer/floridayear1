/* combiner.c
 * Christopher Brant
 * Programming Assignment 1
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 1/23/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// MUST USE "FORK", "EXEC", "PIPE", AND "DUP2" SYSTEM CALLS EFFECTIVELY

// Main function
int main(int argc, char *argv[])
{
	// Parent process will be the mapper
	// Parent runs mapper after creating a pipe
	// Parent writes to the pipe until it is done

	// Child process will be the reducer
	// Child reads from the pipe and processes to stdout













	return 0;
}