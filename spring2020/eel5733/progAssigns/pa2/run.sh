#!/bin/bash
# run.sh
# Christopher Brant
# Programming Assignment 2
# EEL 5733 Advanced Systems Programming
# University of Florida
# 2/10/20

if [[ "$#" -ne 4 || "$1" = "" || "$2" = "" || "$3" = "" || "$4" = "" ]]; then
	echo "Usage: ./run.sh <bufSlots> <numBufs> <inputFileName> <outputFileName>"
	exit 2
fi

# make all the files just in case
make > /dev/null

# run the combiner and pipe the output to a test output file
./combiner $1 $2 < $3 > testOutput.txt
sleep 1

# run diff to check if the expected output matches the test output
differences=$(diff <(sort testOutput.txt) <(sort $4))
if [[ $differences ]]; then
	echo "Differences found in the two files."
	diff <(sort testOutput.txt) <(sort $4)
else
	echo "Test was successful! No errors found."
fi

# clean all the made files and delete testOutput.txt
make clean > /dev/null
rm testOutput.txt
