import time, datetime, config, BlockLayer, InodeOps, MemoryInterface, InodeLayer

#HANDLE OF BLOCK LAYER
blockInt = BlockLayer.BlockLayer()
interface = InodeLayer.InodeLayer()

# Create an inode and fill two blocks with all As and all Bs
inode1 = interface.new_inode(0)
block1 = blockInt.get_valid_data_block()
block2 = blockInt.get_valid_data_block()
block1contents = ["A"] * 512
block2contents = ["B"] * 512
blockInt.update_data_block(block1, block1contents)
blockInt.update_data_block(block2, block2contents)
inode1.blk_numbers[0] = block1
inode1.blk_numbers[1] = block2
inode1.size = 512 * 2
inode1.time_accessed = str(datetime.datetime.now())[:19]
inode1.time_modified = str(datetime.datetime.now())[:19]

# Print the memory system
print(MemoryInterface.status())

# Wait a bit so we can see a difference in access and modified times
#time.sleep(5)

# Call the copy function
inode2 = interface.copy(inode1)
initBlockNumLength1 = len(inode1.blk_numbers) - inode1.blk_numbers.count(-1)

# Print the memory system again
print(MemoryInterface.status())

# Print Check for copy function success
# print "\nInode 1 info below\n"
# print inode1.type
# print "\n"
# print inode1.blk_numbers
# print "\n"
# print inode1.time_accessed
# print "\n"
# print inode1.time_modified
# print "\n"
# print inode1.size
# print "\n"

# print "\nInode 2 info below\n"
# print inode2.type
# print "\n"
# print inode2.blk_numbers
# print "\n"
# print inode2.time_accessed
# print "\n"
# print inode2.time_modified
# print "\n"
# print inode2.size
# print "\n"

# Wait so access times show a difference
#time.sleep(5)

# Down here we will check the read function
#inode1, readData1 = interface.read(inode1, 1030, 2) # this one throws an error as its offset is greater than the file size
# inode1, readData1 = interface.read(inode1, 511, 2)
# inode2, readData2 = interface.read(inode2, 768, 512)

# # Print the memory system
# print(MemoryInterface.status())

# # Print the data in each read buffer
# print "\nreadData 1 should contain 1 A and 1 B below:\n"
# print len(readData1)
# print readData1

# print "\nreadData 2 should contain 512 Bs below:\n"
# print len(readData2)
# print readData2

# Now we test the write functionality
# We take a data buffer of 256 characters all "C"
writeData1 = ["C"] * 256

# # # Write to inode 1 at offset that just writes to a new block
# inode1 = interface.write(inode1, 896, writeData1)

# # Print memory system
# print(MemoryInterface.status())

# # Print Check for copy function success
# print "\nInode 1 info below\n"
# print inode1.type
# print "\n"
# print inode1.blk_numbers
# print "\n"
# print inode1.time_accessed
# print "\n"
# print inode1.time_modified
# print "\n"
# print inode1.size
# print "\n"

# inode1, readData3 = interface.read(inode1, 896, 300)

# print len(readData3)
# print readData3

# checkBlockContents = blockInt.BLOCK_NUMBER_TO_DATA_BLOCK(inode1.blk_numbers[2])
# print checkBlockContents.count("C")
# print checkBlockContents.count(" ")


# Check if we preserve data if written in the middle
#writeData2 = ["C"] * 128

inode1 = interface.write(inode1, 128, writeData1)

print(MemoryInterface.status())
print inode1.blk_numbers
print inode1.size

print "\n"
print inode1.blk_numbers
print inode2.blk_numbers
test = blockInt.BLOCK_NUMBER_TO_DATA_BLOCK(8)
#print test
interface.free_data_block(inode2, 1)
test2 = blockInt.BLOCK_NUMBER_TO_DATA_BLOCK(11)
print test2
print inode2.blk_numbers
print(MemoryInterface.status())


print "\n\n\n\n"
inode2 = interface.write(inode2, 22, "Well hello there")
print(MemoryInterface.status())
inode2, testRead = interface.read(inode2, 0, 80)
print "'" + testRead + "'"
print len(testRead)
print inode2.size












