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
		# Marshal the data to be sent
		m_inode_number = # marshalled inode_number

		# Call the server inode_number_to_inode after marshalling the data correctly
		m_inode = proxy.inode_number_to_inode(m_inode_number)

		# Marshal the retData after returning from the server call
		inode = # unmarshalled inode object

		# Return the correct data
	    return inode


	#REQUEST THE DATA FROM THE SERVER
	def get_data_block(block_number):
		# Marshal the data to be sent
		m_block_number = # marshalled block_number

		# Call the server get_data_block after marshalling the data correctly
		m_block_data = proxy.get_data_block(block_number)

		# Marshal the retData after returning from the server call
		block_data = # unmarshalled block data

		# Return the correct data
	    return block_data


	#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
	def get_valid_data_block():
	    # Call the server get_valid_data_block
	    m_valid_block = proxy.get_valid_data_block()

		# Marshal the retData after returning from the server call
		valid_block = # unmarshalled valid block number

		# Return the correct data
	    return valid_block


	#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
	def free_data_block(block_number):
		# Marshal the data to be sent
		m_block_number = # marshalled block_number

	    # Call the server free_data_block after marshalling the data correctly
	    m_retErr = proxy.free_data_block(m_block_number)

	    # Marshal the retErr after returning from the server call
	    retErr = # unmarshall m_retErr

	    # Return the possible error, if no error, set retErr to 0
	    return retErr


	#REQUEST TO WRITE DATA ON THE THE SERVER
	def update_data_block(block_number, block_data):
		# Marshal the data to be sent
		m_block_number = # marshalled block_number
		m_block_data = # marshalled block_data

	    # Call the server update_data_block after marshalling the data correctly
	    m_retErr = proxy.update_data_block(m_block_number, m_block_data)

		# Marshal the retErr after returning from the server call
	    retErr = # unmarshall m_retErr

		# Return the possible error, if no error, set retErr to 0
	    return retErr


	#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
	def update_inode_table(inode, inode_number):
		# Marshal the data to be sent
		m_inode = # marshalled inode
		m_inode_number = # marshalled inode_number

	    # Call the server update_inode_table after marshalling the data correctly
	    m_retErr = proxy.update_inode_table(m_inode, m_inode_number)


		# Marshal the retErr after returning from the server call
	    retErr = # unmarshall m_retErr

		# Return the possible error, if no error, set retErr to 0
	    return retErr


	#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
	def status():
	    # Call the server status
	    m_statusString = proxy.status()

		# Marshal the retData after returning from the server call
		statusString = # unmarshalled m_statusString

		# Print out the status to the terminal
		print statusString












	    


