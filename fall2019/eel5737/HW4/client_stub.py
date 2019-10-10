# SKELETON CODE FOR CLIENT STUB HW4
import xmlrpclib, config, pickle

class client_stub():

	def __init__(self):
		self.proxy = xmlrpclib.ServerProxy("http://localhost:8000/")


	# DEFINE FUNCTIONS HERE

	# example provided for initialize
	def Initialize(self):
		try :
			self.proxy.Initialize()
		except Exception as err :
			# print error message
			quit()


	# REPLACE EVERYTHING BELOW HERE

	#REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
	def inode_number_to_inode(inode_number):
	    return client_stub.inode_number_to_inode(inode_number)


	#REQUEST THE DATA FROM THE SERVER
	def get_data_block(block_number):
	    return ''.join(client_stub.get_data_block(block_number))


	#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
	def get_valid_data_block():
	    return ( client_stub.get_valid_data_block() )


	#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
	def free_data_block(block_number):
	    client_stub.free_data_block((block_number))


	#REQUEST TO WRITE DATA ON THE THE SERVER
	def update_data_block(block_number, block_data):
	    client_stub.update_data_block(block_number, block_data)


	#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
	def update_inode_table(inode, inode_number):
	    client_stub.update_inode_table(inode, inode_number)


	#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
	def status():
	    return client_stub.status()


