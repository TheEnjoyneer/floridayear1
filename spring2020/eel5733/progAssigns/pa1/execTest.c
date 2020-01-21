// Test file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	
	//execlp("./mapper", "mapper", "input.txt", NULL);

	execlp("./helloWorld", "helloWorld", NULL, NULL);

	return 0;
}