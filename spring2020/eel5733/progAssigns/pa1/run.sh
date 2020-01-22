#/bin/bash
# run.sh
# Christopher Brant
# Programming Assignment 1
# EEL 5733 Advanced Systems Programming
# University of Florida
# 1/23/20

# run the combiner and pipe the output to a test output file
./combiner input.txt > testOutput.txt

# run diff to check if the expected output matches the test output
diff testOutput.txt output.txt