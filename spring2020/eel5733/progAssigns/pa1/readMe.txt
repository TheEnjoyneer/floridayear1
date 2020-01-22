# readMe.txt
# Christopher Brant
# Programming Assignment 1
# EEL 5733 Advanced Systems Programming
# University of Florida
# 1/23/20

For Testing My Program:
	Prerequisites
	- A text file of tuples for input
	- A text file of tuples of expected output
	- All 3 files compiled using the makefile already

	Running
	1) Run "run.sh <inputFileName> <outputFileName>"

	* If the script runs with zero output, the program worked correctly *
	* The "run.sh" script runs the combiner program and pipes the output
	  to a file called "testOutput.txt" and then runs "diff" to check the
	  difference in the "testOutput.txt" contents and the contents of the
	  expected output file *

For Running My Program Normally:
	Prerequisites
	- A text file of tuples for input
	- All 3 files compiled using the makefile already

	Running
	1) Run "./combiner <inputFileName>"

	* This will run the two programs in their processes and output the 
	  tuples to the screen on stdout *