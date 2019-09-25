# fixTest.py

import datetime, config, BlockLayer, InodeOps, MemoryInterface, InodeLayer, time

interface = BlockLayer.BlockLayer()
inodeInt = InodeLayer.InodeLayer()

print(MemoryInterface.status())

testInode = inodeInt.new_inode(0)

# Indicate the running test
print "\nRUNNING TEST 1:\n"

# Write and print the memory interface status
testInode = inodeInt.write(testInode, 0, "01234567")

print(MemoryInterface.status())
time.sleep(1)

# Read the file just written to
testInode, testRead = inodeInt.read(testInode, 0, 8)

# Print the read results
print "\n"
print "'" + testRead + "'"
print "\n"

# Indicate the running test
print "\nRUNNING TEST 2:\n"

copyInode = inodeInt.copy(testInode)
time.sleep(1)

# Read the copy
copyInode, testRead = inodeInt.read(copyInode, 0, 8)

# Printing output
print "'" + testRead + "'"

print "\ntestInode's blk_numbers:"
print testInode.blk_numbers
print "\n"

print "\ncopyInode's blk_numbers:"
print copyInode.blk_numbers
print "\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 3:\n"

# Read where indicated
testInode, testRead = inodeInt.read(testInode, 2, 2)
time.sleep(1)

# Print the output
print "'" + testRead + "'"
print"\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 4:\n"

# Write to the file
testInode = inodeInt.write(testInode, 1, "writehere")
time.sleep(1)

# Read that write
testInode, testRead = inodeInt.read(testInode, 0, 10)

# print the output
print "'" + testRead + "'"
print "\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 5:\n"

# perform the write
testInode = inodeInt.write(testInode, 0, "reset\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
time.sleep(1)

# read the write
testInode, testRead = inodeInt.read(testInode, 0, 5)

# print the output
print "Length of reset string is: ", len("reset\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00") 
print "'" + testRead + "'"
print "\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 6:\n"

# perform the write
testInode = inodeInt.write(testInode, 5, " append")
time.sleep(1)
print "File size: ", testInode.size
print testInode.blk_numbers

# read the write
testInode, testRead = inodeInt.read(testInode, 0, 12)

# print the output
print "'" + testRead + "'"
print "\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 7:\n"

# perform the write
testInode = inodeInt.write(testInode, 0, "0000    0002    0004    0006    ") 
time.sleep(1)

# read the write
testInode, testRead = inodeInt.read(testInode, 0, 16)

# print the output
print "Length of file + write at offset: ", len("0000    0002    0004    0006    ")
print testInode.blk_numbers
print "'" + testRead + "'"
print "\n"

print(MemoryInterface.status())


# Indicate the running test
print "\nRUNNING TEST 8:\n"

# print the output of the time accessed, modified, and created for each inode
print "testInode info:"
print "testInode.time_created: " + testInode.time_created
print "testInode.time_accessed: " + testInode.time_accessed
print "testInode.time_modified: " + testInode.time_modified
print "\ncopyInode info:"
print "copyInode.time_created: " + copyInode.time_created
print "copyInode.time_accessed: " + copyInode.time_accessed
print "copyInode.time_modified: " + copyInode.time_modified
print "\n"















