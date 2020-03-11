/* combiner.c
 * Christopher Brant
 * Programming Assignment 4
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 3/14/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Main function
int main(int argc, char *argv[])
{
	// Declare necessary variables, create pipe, and fork the process
	int fd[2];
	pipe(fd);
	pid_t pid = fork();

	// Code for the Child process to run reducer
	if (pid == 0)
	{
		// Close the write end of the pipe
		close(fd[1]);

		// Redirect the reducer's "stdin" to come from the read end of the pipe
		dup2(fd[0], STDIN_FILENO);

		// Execute the reducer program
		execlp("./reducer", "reducer", NULL, NULL);
	}

	// Code for the Parent process to run mapper
	else
	{
		// Close the read end of the pipe
		close(fd[0]);

		// Redirect the mapper's "stdout" to go to the write end of the pipe
		dup2(fd[1], STDOUT_FILENO);

		// Execute the mapper program
		execlp("./mapper", "mapper", argv[1], NULL);
	}

	return 0;
}