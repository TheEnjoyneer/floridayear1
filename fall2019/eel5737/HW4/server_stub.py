# SKELETON CODE FOR SERVER STUB HW4
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer

import time, Memory, pickle , InodeOps, config, DiskLayout


filesystem = Memory.Operations()

# FUNCTION DEFINITIONS 

# example provided for initialize
def Initialize():
	retVal = Memory.Initialize()
	retVal = pickle.dumps(retVal)
	return retVal

	# EVERYTHING BELOW HERE IS WHAT WILL NEED TO BE CHANGED
	# HOWEVER DO SPECIFICALLY USE THESE FUNCTION NAMES

	#GIVES ADDRESS OF INODE TABLE
	def addr_inode_table(self):				
		return sblock.ADDR_INODE_BLOCKS


	#RETURNS THE DATA OF THE BLOCK
	def get_data_block(self, block_number):	
		if block_number == 0: print("Memory: Reserved for Boot Block")
		elif block_number == 1: print("Memory: Reserved for Super Block")
		elif block_number >= sblock.BITMAP_BLOCKS_OFFSET and block_number < sblock.INODE_BLOCKS_OFFSET:
			return sblock.ADDR_BITMAP_BLOCKS[block_number - sblock.BITMAP_BLOCKS_OFFSET].block
		elif block_number >= sblock.INODE_BLOCKS_OFFSET and block_number < sblock.DATA_BLOCKS_OFFSET:
			return sblock.ADDR_INODE_BLOCKS[block_number - sblock.INODE_BLOCKS_OFFSET].block
		elif block_number >= sblock.DATA_BLOCKS_OFFSET and block_number < sblock.TOTAL_NO_OF_BLOCKS:
			return sblock.ADDR_DATA_BLOCKS[block_number - sblock.DATA_BLOCKS_OFFSET].block
		else: print("Memory: Block index out of range or Wrong input!")
		return -1


	#RETURNS THE BLOCK NUMBER OF AVAIALBLE DATA BLOCK  
	def get_valid_data_block(self):			
		for i in range(0, sblock.TOTAL_NO_OF_BLOCKS):
			if sblock.ADDR_BITMAP_BLOCKS[i / sblock.BLOCK_SIZE].block[i % sblock.BLOCK_SIZE] == 0:
				sblock.ADDR_BITMAP_BLOCKS[i / sblock.BLOCK_SIZE].block[i % sblock.BLOCK_SIZE] = 1
				return i
		print("Memory: No valid blocks available")
		return -1

	#REMOVES THE INVALID DATA BLOCK TO MAKE IT REUSABLE
	def free_data_block(self, block_number):  	
		if block_number > 0:
			sblock.ADDR_BITMAP_BLOCKS[block_number / sblock.BLOCK_SIZE].block[block_number % sblock.BLOCK_SIZE] = 0
			b = sblock.ADDR_DATA_BLOCKS[block_number - sblock.DATA_BLOCKS_OFFSET].block
			for i in range(0, sblock.BLOCK_SIZE): b[i] = '\0'


	#WRITES TO THE DATA BLOCK
	def update_data_block(self, block_number, block_data):		
		b = sblock.ADDR_DATA_BLOCKS[block_number - sblock.DATA_BLOCKS_OFFSET].block
		for i in range(0, len(block_data)): b[i] = block_data[i]
		#print("Memory: Data Copy Completes")
	
	
	#UPDATES INODE TABLE WITH UPDATED INODE
	def update_inode_table(self, inode, inode_number):
		sblock.ADDR_INODE_BLOCKS[inode_number / sblock.INODES_PER_BLOCK].block[inode_number % sblock.INODES_PER_BLOCK] = inode

	
	#RETURNS THE INODE FROM INODE NUMBER
	def inode_number_to_inode(self, inode_number):
		return sblock.ADDR_INODE_BLOCKS[inode_number / sblock.INODES_PER_BLOCK].block[inode_number % sblock.INODES_PER_BLOCK]

	
	#SHOWS THE STATUS OF DISK LAYOUT IN MEMORY
	def status(self):
		counter = 1
		string = ""
		string += "\n----------BITMAP: ----------(Block Number : Valid Status)\n"
		block_number = 0
		for i in range(2, sblock.INODE_BLOCKS_OFFSET):
			string += "Bitmap Block : " + str(i - 2) + "\n"
			b = sblock.ADDR_BITMAP_BLOCKS[i - sblock.BITMAP_BLOCKS_OFFSET].block
			for j in range(0, len(b)):
				if j == 20: break   #only to avoid useless data to print
				string += "\t\t[" + str(block_number + j) + "  :  "  + str(b[j]) + "]  \n"
			block_number += len(b)
			if counter == 1: break
		string += ".....showing just part(20) of 1st bitmap block!\n"

		string += "\n\n----------INODE Blocks: ----------(Inode Number : Inode(Address)\n"
		inode_number = 0
		for i in range(sblock.INODE_BLOCKS_OFFSET, sblock.DATA_BLOCKS_OFFSET):
			string += "Inode Block : " + str(i - sblock.INODE_BLOCKS_OFFSET) + "\n"
			b = sblock.ADDR_INODE_BLOCKS[i - sblock.INODE_BLOCKS_OFFSET].block
			for j in range(0, len(b)):
				string += "\t\t[" + str(inode_number + j) + "  :  "  + str(bool(b[j])) + "]  \n"
			inode_number += len(b)
		
		string += "\n\n----------DATA Blocks: ----------\n  "
		counter = 0
		for i in range(sblock.DATA_BLOCKS_OFFSET, sblock.TOTAL_NO_OF_BLOCKS):
			if counter == 25: 
				string += "......Showing just part(25) data blocks\n"
				break
			string += (str(i) + " : " + "".join(sblock.ADDR_DATA_BLOCKS[i - sblock.DATA_BLOCKS_OFFSET].block)) + "  "
			counter += 1

		
		string += "\n\n----------HIERARCHY: ------------\n"
		for i in range(sblock.INODE_BLOCKS_OFFSET, sblock.DATA_BLOCKS_OFFSET):
			for j in range(0, sblock.INODES_PER_BLOCK):
				inode = sblock.ADDR_INODE_BLOCKS[i-sblock.INODE_BLOCKS_OFFSET].block[j]
				if inode and inode[0]:
					string += "\nDIRECTORY: " + inode[1] + "\n"
					for x in inode[7]: string += "".join(x[:config.MAX_FILE_NAME_SIZE]) + " || "
					string += "\n"
		
		return string


# Begin server listening
server = SimpleXMLRPCServer(("",8000))
print ("Listening on port 8000...")

# Registering all server functions below
server.register_function(Initialize, 		   	"Initialize")
server.register_function(addr_inode_table, 		"addr_inode_table")
server.register_function(get_data_block, 		"get_data_block")
server.register_function(get_valid_data_block, 	"get_valid_data_block")
server.register_function(free_data_block, 		"free_data_block")
server.register_function(update_data_block, 	"update_data_block")
server.register_function(update_inode_table, 	"update_inode_table")
server.register_function(inode_number_to_inode, "inode_number_to_inode")
server.register_function(status, 		   		"status")

# Run the server
server.serve_forever()














