# Python Test Script

import InodeOps

#MAKES NEW INODE OBJECT
def new_inode(type):
    return InodeOps.Table_Inode(type)

#FLUSHES ALL THE BLOCKS OF INODES FROM GIVEN INDEX OF MAPPING ARRAY  
def free_data_block(inode, index):
	for i in range(index, len(inode.blk_numbers)):
		#interface.free_data_block(inode.blk_numbers[i])
		inode.blk_numbers[i] = -1

if __name__ == "__main__":
	# Create an inode variable
	inode = new_inode(0)

	# Set blk_numbers before clearing
	inode.blk_numbers[0] = 10
	inode.blk_numbers[1] = 11
	inode.blk_numbers[2] = 12
	inode.blk_numbers[3] = 13
	inode.blk_numbers[4] = 14
	inode.blk_numbers[5] = 15
	inode.blk_numbers[6] = 16
	inode.blk_numbers[7] = 17

	free_data_block(inode, 6)

	for i in range(len(inode.blk_numbers)):
		print inode.blk_numbers[i]


