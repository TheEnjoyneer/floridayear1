# client_stub.py
# Christopher Brant
# University of Florida
# EEL 5737 PoCSD
# HW4 Part B
import xmlrpclib, config, pickle

class client_stub():

	def __init__(self):
		self.proxy = xmlrpclib.ServerProxy("http://localhost:8000/")

	# example provided for initialize
	def Initialize(self):
		try :
			self.proxy.Initialize()
		except Exception as err :
			# print error message
			quit()


	#REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
	def inode_number_to_inode(inode_number):
		# Return the correct data
	    return pickle.loads(proxy.inode_number_to_inode(pickle.dumps(inode_number)))


	#REQUEST THE DATA FROM THE SERVER
	def get_data_block(block_number):
		# Return the correct data
	    return pickle.loads(proxy.get_data_block(pickle.dumps(block_number)))


	#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
	def get_valid_data_block():
		# Return the correct data
	    return pickle.loads(proxy.get_valid_data_block())


	#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
	def free_data_block(block_number):
	    # Return the possible error, if no error, set retErr to 0
	    return pickle.loads(proxy.free_data_block(pickle.dumps(block_number)))


	#REQUEST TO WRITE DATA ON THE THE SERVER
	def update_data_block(block_number, block_data):
		# Return the possible error, if no error, set retErr to 0
	    return pickle.loads(proxy.update_data_block(pickle.dumps(block_number), pickle.dumps(block_data)))


	#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
	def update_inode_table(inode, inode_number):
		# Return the possible error, if no error, set retErr to 0
	    return pickle.loads(proxy.update_inode_table(pickle.dumps(inode), pickle.dumps(inode_number)))


	#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
	def status():
		# Return the status string after marshalling the data
		return pickle.loads(proxy.status())












	    


