# client_stub.py
# Christopher Brant
# University of Florida
# EEL 5737 PoCSD
# HW4 Part B
import xmlrpclib, config, pickle, time

class client_stub():

	def __init__(self):
		self.proxy = xmlrpclib.ServerProxy("http://localhost:8000/")

	# CLIENT REQUEST TO INITIALIZE THE MEMORY SYSTEM
	def Initialize(self):
		try:
			self.proxy.Initialize()
		except Exception as err:
			# print error message
				print "Error in re-initializing the filesystem."
				quit()


	#REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
	def inode_number_to_inode(self, inode_number):
		# Return the correct data
		try:
			return pickle.loads(self.proxy.inode_number_to_inode(pickle.dumps(inode_number)))
		except xmlrpclib.Error as err:
			print "A fault occurred in client_stub.inode_number_to_inode()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUEST THE DATA FROM THE SERVER
	def get_data_block(self, block_number):
		# Return the correct data
		try:
			return pickle.loads(self.proxy.get_data_block(pickle.dumps(block_number)))
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.get_data_block()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
	def get_valid_data_block(self):
		# Return the correct data
		try:
			return pickle.loads(self.proxy.get_valid_data_block())
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.get_valid_data_block()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
	def free_data_block(self, block_number):
	    # Return the possible error, if no error, set retErr to 0
		try:
			return pickle.loads(self.proxy.free_data_block(pickle.dumps(block_number)))
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.free_data_block()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUEST TO WRITE DATA ON THE THE SERVER
	def update_data_block(self, block_number, block_data):
		# Return the possible error, if no error, set retErr to 0
		try:
			return pickle.loads(self.proxy.update_data_block(pickle.dumps(block_number), pickle.dumps(block_data)))
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.update_data_block()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
	def update_inode_table(self, inode, inode_number):
		# Return the possible error, if no error, set retErr to 0
		try:
			return pickle.loads(self.proxy.update_inode_table(pickle.dumps(inode), pickle.dumps(inode_number)))
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.update_inode_table()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()


	#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
	def status(self):
		# Return the status string after marshalling the data
		try:
			return pickle.loads(self.proxy.status())
		except xmlrpclib.Fault as err:
			print "A fault occurred in client_stub.status()"
			print "Fault code: %d" % err.faultCode
			print "Fault string: %s" % err.faultString
			quit()












	    


