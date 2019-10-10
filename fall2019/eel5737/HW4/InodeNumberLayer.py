'''
THIS MODULE ACTS AS A INODE NUMBER LAYER. NOT ONLY IT SHARES DATA WITH INODE LAYER, BUT ALSO IT CONNECTS WITH MEMORY INTERFACE FOR INODE TABLE 
UPDATES. THE INODE TABLE AND INODE NUMBER IS UPDATED IN THE FILE SYSTEM USING THIS LAYER
'''
import InodeLayer, config, MemoryInterface, datetime, InodeOps, MemoryInterface


#HANDLE OF INODE LAYER
interface = InodeLayer.InodeLayer()

class InodeNumberLayer():

	#PLEASE DO NOT MODIFY
	#ASKS FOR INODE FROM INODE NUMBER FROM MemoryInterface.(BLOCK LAYER HAS NOTHING TO DO WITH INODES SO SEPERTAE HANDLE)
	def INODE_NUMBER_TO_INODE(self, inode_number):
		array_inode = MemoryInterface.inode_number_to_inode(inode_number)
		inode = InodeOps.InodeOperations().convert_array_to_table(array_inode)
		if inode: inode.time_accessed = datetime.datetime.now()   #TIME OF ACCESS
		return inode


	#PLEASE DO NOT MODIFY
	#RETURNS DATA BLOCK FROM INODE NUMBER
	def INODE_NUMBER_TO_BLOCK(self, inode_number, offset, length):
		inode = self.INODE_NUMBER_TO_INODE(inode_number)
		if not inode:
			print("Error InodeNumberLayer: Wrong Inode Number! \n")
			return -1
		return interface.read(inode, offset, length)


	#PLEASE DO NOT MODIFY
	#UPDATES THE INODE TO THE INODE TABLE
	def update_inode_table(self, table_inode, inode_number):
		if table_inode: table_inode.time_modified = datetime.datetime.now()  #TIME OF MODIFICATION 
		array_inode = InodeOps.InodeOperations().convert_table_to_array(table_inode)
		MemoryInterface.update_inode_table(array_inode, inode_number)


	#PLEASE DO NOT MODIFY
	#FINDS NEW INODE INODE NUMBER FROM FILESYSTEM
	def new_inode_number(self, type, parent_inode_number, name):
		if parent_inode_number != -1:
			parent_inode = self.INODE_NUMBER_TO_INODE(parent_inode_number)
			if not parent_inode:
				print("Error InodeNumberLayer: Incorrect Parent Inode")
				return -1
			entry_size = config.MAX_FILE_NAME_SIZE + len(str(config.MAX_NUM_INODES))
			max_entries = (config.INODE_SIZE - 79 ) / entry_size
			if len(parent_inode.directory) == max_entries:
				print("Error InodeNumberLayer: Maximum inodes allowed per directory reached!")
				return -1
		for i in range(0, config.MAX_NUM_INODES):
			if self.INODE_NUMBER_TO_INODE(i) == False: #FALSE INDICTES UNOCCUPIED INODE ENTRY HENCE, FREEUMBER
				inode = interface.new_inode(type)
				inode.name = name
				self.update_inode_table(inode, i)
				return i
		print("Error InodeNumberLayer: All inode Numbers are occupied!\n")


	#LINKS THE INODE
	def link(self, file_inode_number, hardlink_name, hardlink_parent_inode_number):

		# Lookup location to add hardlink_name to
		file_inode = self.INODE_NUMBER_TO_INODE(file_inode_number)
		hardlink_parent_inode = self.INODE_NUMBER_TO_INODE(hardlink_parent_inode_number)

		# Ensure the inodes are valid before using them
		if (hardlink_parent_inode) == False or (file_inode == False):
			print "\nError: Parent inode or file inode number supplied is invalid.\n"
			return -1

		# Add link to directory in new location
		hardlink_parent_inode.directory[hardlink_name] = file_inode_number

		# Increment file_inode ref count
		file_inode.links += 1

		# Update the inode table with the new values necessary
		self.update_inode_table(hardlink_parent_inode, hardlink_parent_inode_number)
		self.update_inode_table(file_inode, file_inode_number)

		# Return if we get here
		return


	#REMOVES THE INODE ENTRY FROM INODE TABLE
	def unlink(self, inode_number, parent_inode_number, filename):

		# Retrieve inode to unlink filename from
		inode = self.INODE_NUMBER_TO_INODE(inode_number)
		parent_inode = self.INODE_NUMBER_TO_INODE(parent_inode_number)

		# Ensure the inodes are valid before using them
		if (parent_inode) == False or (inode == False):
			print "\nError: Parent inode or file inode number supplied is invalid.\n"
			return -1

		# Check if we need to free the inode and do so if necessary
		if inode.type == 1: # If inode is a directory
			if (inode.links - 1) == 1:
				# Check if directory is empty
				if len(inode.directory) == 0:
					# Remove the filename from the parent_inode
					del parent_inode.directory[filename]
					# if empty, free all blocks in inode, and free the inode
					interface.free_data_block(inode, 0)
					inode = False
				else:  # If not free return error with message of non-empty directory unlink attempt
					print "\nError: Attempt to remove a the last link to a non-empty directory."
					return -1
			else:
				# Remove the filename from the parent_inode
				del parent_inode.directory[filename]
				# Decrement reference count for inode
				inode.links -= 1


		elif inode.type == 0: # If inode is a file
			if (inode.links - 1) == 0:
				# Remove the filename from the parent_inode
				del parent_inode.directory[filename]
				# Free all blocks and free the inode
				interface.free_data_block(inode, 0)
				inode = False
			else:
				# Remove the filename from the parent_inode
				del parent_inode.directory[filename]
				# Decrement reference count for inode
				inode.links -= 1

		else: # If inode is not a file or directory (for now) return error
			print "\nGiven inode is of a type: ", inode.type, "and is not acceptable in this system."
			return -1

		# Update parent inode and the file inode in the inode table
		self.update_inode_table(parent_inode, parent_inode_number)
		self.update_inode_table(inode, inode_number)

		# Return if it gets this far
		return True


	#IMPLEMENTS WRITE FUNCTIONALITY
	def write(self, inode_number, offset, data, parent_inode_number):

		# Retrieve parent inode and file inode
		parent_inode = self.INODE_NUMBER_TO_INODE(parent_inode_number)
		inode = self.INODE_NUMBER_TO_INODE(inode_number)

		# Ensure the inodes are valid before using them
		if (parent_inode) == False or (inode == False):
			print "\nError: Parent inode or file inode number supplied is invalid.\n"
			return -1

		# Check if the inode exists in the parent directory
		if inode_number in parent_inode.directory.values():
			# Check inode type
			if inode.type != 0:
				print "\nError: inode is not of type: file.\n"
				return -1
			else:
				# Call the InodeLayer write function
				inode = interface.write(inode, offset, data)

				# Check for errors
				if inode == -1:
					print "\nError in InodeLayer write to inode_number: ", inode_number, "\n"
					return -1
				else:
					# Update the inode table
					self.update_inode_table(inode, inode_number)
					self.update_inode_table(parent_inode, parent_inode_number)

					# Return
					return True
		else:
			# if the inode does not exist in the parent directory return error
			print "\nError: Given inode number does not have a binding in the given parent inode number's context.\n"
			return -1


	#IMPLEMENTS READ FUNCTIONALITY
	def read(self, inode_number, offset, length, parent_inode_number):

		# Retrieve parent inode and file inode
		parent_inode = self.INODE_NUMBER_TO_INODE(parent_inode_number)
		inode = self.INODE_NUMBER_TO_INODE(inode_number)

		# Ensure the inodes are valid before using them
		if (parent_inode) == False or (inode == False):
			print "\nError: Parent inode or file inode number supplied is invalid.\n"
			return -1

		# Check if the inode exists in the parent directory
		if inode_number in parent_inode.directory.values():
			# Check inode type
			if inode.type != 0:
				print "\nError: inode is not of type: file.\n"
				return -1
			else:
				# Call the InodeLayer read function
				inode, retData = interface.read(inode, offset, length)

				if inode == -1:
					print "\nError in InodeLayer read from inode_number: ", inode_number, "\n"
					return -1
				else:
					# Update the inode table
					self.update_inode_table(inode, inode_number)
					self.update_inode_table(parent_inode, parent_inode_number)

					# Return the data read
					return retData

		else:
			# if the 
			print "\nError: Given inode number does not have a binding in the given parent inode number's context.\n"
			return -1









