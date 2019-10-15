# server_stub.py
# Christopher Brant
# University of Florida
# EEL 5737 PoCSD
# HW4 Part B
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer

import time, Memory, pickle , InodeOps, config

filesystem = Memory.Operations()

# FUNCTION DEFINITIONS 
# INITIALIZE THE FILESYSTEM
def Initialize():
	# Marshal the response of Memory.Initialize()
	retVal = Memory.Initialize()
	retVal = pickle.dumps(retVal)
	return retVal


#GIVES ADDRESS OF INODE TABLE
# Not so sure this needs to be implemented in this specific version
def addr_inode_table():				
	return pickle.dumps(sblock.ADDR_INODE_BLOCKS)


#RETURNS THE DATA OF THE BLOCK
def get_data_block(block_number):	
	# Unmarshal the data for block_number
	return pickle.dumps(filesystem.get_data_block(pickle.loads(block_number)))


#RETURNS THE BLOCK NUMBER OF AVAIALBLE DATA BLOCK  
def get_valid_data_block():
	# Unmarshal the data and call the Memory Operations function
	return pickle.dumps(filesystem.get_valid_data_block())	


#REMOVES THE INVALID DATA BLOCK TO MAKE IT REUSABLE
def free_data_block(block_number):
	# Marshal the input and call filesystem
	return pickle.dumps(filesystem.free_data_block(pickle.loads(block_number)))


#WRITES TO THE DATA BLOCK
def update_data_block(block_number, block_data):
	# Marshal the input and call filesystem
	return pickle.dumps(filesystem.update_data_block(pickle.loads(block_number), pickle.loads(block_data)))


#UPDATES INODE TABLE WITH UPDATED INODE
def update_inode_table(inode, inode_number):
	# Marshal the input and call filesystem
	return pickle.dumps(filesystem.update_inode_table(pickle.loads(inode), pickle.loads(inode_number)))
	

#RETURNS THE INODE FROM INODE NUMBER
def inode_number_to_inode(inode_number):
	# Marshal the input and call filesystem
	return pickle.dumps(filesystem.inode_number_to_inode(pickle.loads(inode_number)))
	

#SHOWS THE STATUS OF DISK LAYOUT IN MEMORY
def status():
	# Marshal the input and call filesystem
	return pickle.dumps(filesystem.status())


# Begin server listening
server = SimpleXMLRPCServer(("",8000))
print ("Listening on port 8000...")

# Registering all server functions below
server.register_multicall_functions()
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














