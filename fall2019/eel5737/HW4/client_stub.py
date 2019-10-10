# client_stub.py
# Christopher Brant
# University of Florida
# EEL 5737 PoCSD
# HW4 Part B
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


	#REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
	def inode_number_to_inode(inode_number):
		# Call the server inode_number_to_inode after marshalling the data correctly


		# Marshal the retData after returning from the server call


		# Return the correct data
	    return retData


	#REQUEST THE DATA FROM THE SERVER
	def get_data_block(block_number):
		# Call the server get_data_block after marshalling the data correctly


		# Marshal the retData after returning from the server call


		# Return the correct data
	    return retData


	#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
	def get_valid_data_block():
	    # Call the server get_valid_data_block after marshalling the data correctly


		# Marshal the retData after returning from the server call


		# Return the correct data
	    return retData


	#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
	def free_data_block(block_number):
	    # Call the server free_data_block after marshalling the data correctly


		# Marshal the retData after returning from the server call




	#REQUEST TO WRITE DATA ON THE THE SERVER
	def update_data_block(block_number, block_data):
	    # Call the server update_data_block after marshalling the data correctly


		# Marshal the retData after returning from the server call




	#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
	def update_inode_table(inode, inode_number):
	    # Call the server update_inode_table after marshalling the data correctly


		# Marshal the retData after returning from the server call




	#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
	def status():
	    # Call the server status after marshalling the data correctly


		# Marshal the retData after returning from the server call


		# Print out the status to the terminal
	    


