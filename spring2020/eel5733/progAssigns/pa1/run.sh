#!/bin/bash
# run.sh
# Christopher Brant
# Programming Assignment 1
# EEL 5733 Advanced Systems Programming
# University of Florida
# 1/23/20

if [[ "$#" -ne 2 || "$1" = "" || "$2" = "" ]]; then
	echo "Usage: ./run.sh <inputFileName> <outputFileName>"
	exit 2
fi

# make all the files just in case
make > /dev/null

# run the combiner and pipe the output to a test output file
./combiner $1 > testOutput.txt
sleep 1

# run diff to check if the expected output matches the test output
differences=$(diff testOutput.txt $2)
if [[ $differences ]]; then
	echo "Differences found in the two files."
	diff testOutput.txt $2
else
	echo "Test was successful! No errors found."
fi

# clean all the made files and delete testOutput.txt
make clean > /dev/null
rm testOutput.txt
