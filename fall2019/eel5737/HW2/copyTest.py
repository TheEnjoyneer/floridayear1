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
print "\nBlock1 of inode1 contains all As, and Block2 of inode1 contains all Bs\n"

print "\n Printing the memory system status now\n"
# Print the memory system
print(MemoryInterface.status())

# Wait a bit so we can see a difference in access and modified times
time.sleep(5)

# Call the copy function
inode2 = interface.copy(inode1)

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
time.sleep(5)

# Down here we will check the read function
readData1 = interface.read(inode1, 0, 1024)
readData2 = interface.read(inode2, 512, 512)

# Print the memory system
print(MemoryInterface.status())

# Print the data in each read buffer
print "\nreadData 1 should contain 512 As and 512 Bs below:\n"
print readData1

print "\nreadData 2 should contain 512 Bs below:\n"
print readData2











