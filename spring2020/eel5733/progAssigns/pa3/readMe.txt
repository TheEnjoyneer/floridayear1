# readMe.txt
# Christopher Brant
# Programming Assignment 2
# EEL 5733 Advanced Systems Programming
# University of Florida
# 2/10/20

For Testing My Program:
	Prerequisites
	- A text file of tuples for input
	- A text file of tuples of expected output

	Running
	1) Run "run.sh <bufSlots> <numBufs> <inputFileName> <outputFileName>"

	* The "run.sh" script runs the combiner program and pipes the output
	  to a file called "testOutput.txt" and then runs "diff" to check the
	  difference in the "testOutput.txt" contents and the contents of the
	  expected output file *
	* One more important note is that the "run.sh" script also re-runs
	  the "make" command to ensure that the binaries are available *

My Testing:
	When testing my program I created a series of different input files and
	corresponding expected output files, and ran each one of them through
	my "run.sh" script such that they were all instantly run and tested for
	correctness at the same time.

	* Important note *
	In my program, if the number of user IDs is greater than the number of
	buffers given in the command line arguments, then those will be entirely
	dismissed and not counted in the reduced output.

For Running My Program Normally:
	Prerequisites
	- A text file of tuples for input
	- All files compiled using the makefile already

	Running
	1) Run "./combiner <bufSlots> <numBufs> < <inputFileName>"

	* This will run the two programs in their processes and output the 
	  tuples to the screen on stdout *