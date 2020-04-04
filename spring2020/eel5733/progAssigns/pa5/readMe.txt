# readMe.txt
# Christopher Brant
# Programming Assignment 5
# EEL 5733 Advanced Systems Programming
# University of Florida
# 4/4/20

For Testing My Program:
	Prerequisites
	- Compiled and loaded my device driver program
	- Compiled my testing program
	- In the case that the device nodes do not automatically create upon program loading,
	  the device nodes have to be manually created at the command line

	Running
	1) Run "./asp_mycdev_test"

	* This function should print 3 statements, should print the Hello statement twice,
	  then should print a statement with the Hello statement and the Crikey statement. *

	* If you run "dmesg -T" you should see that it wrote, read, read, wrote, and read once more,
	  then it will close all devices.  My test program opens them all but only reads and writes to
	  the first one. *

	* Dr. Yavuz told emailed me and told me that testing can be done with nodes that are created
	  manually if the dynamic creation on loading isn't working. *