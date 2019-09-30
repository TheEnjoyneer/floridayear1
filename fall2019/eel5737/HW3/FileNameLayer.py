'''
THIS MODULE ACTS LIKE FILE NAME LAYER AND PATH NAME LAYER (BOTH) ABOVE INODE LAYER.
IT RECIEVES INPUT AS PATH (WITHOUT INITIAL '/'). THE LAYER IMPLEMENTS LOOKUP TO FIND INODE NUMBER OF THE REQUIRED DIRECTORY.
PARENTS INODE NUMBER IS FIRST EXTRACTED BY LOOKUP AND THEN CHILD INODE NUMBER BY RESPECTED FUNCTION AND BOTH OF THEM ARE UPDATED
'''
import InodeNumberLayer, os

#HANDLE OF INODE NUMBER LAYER
interface = InodeNumberLayer.InodeNumberLayer()

class FileNameLayer():

	#PLEASE DO NOT MODIFY
	#RETURNS THE CHILD INODE NUMBER FROM THE PARENTS INODE NUMBER
	def CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(self, childname, inode_number_of_parent):
		inode = interface.INODE_NUMBER_TO_INODE(inode_number_of_parent)
		if not inode: 
			print("Error FileNameLayer: Lookup Failure!")
			return -1
		if inode.type == 0:
			print("Error FileNameLayer: Invalid Directory!")
			return -1
		if childname in inode.directory: return inode.directory[childname]
		print("Error FileNameLayer: Lookup Failure!")
		return -1

	#PLEASE DO NOT MODIFY
	#RETUNS THE PARENT INODE NUMBER FROM THE PATH GIVEN FOR A FILE/DIRECTORY 
	def LOOKUP(self, path, inode_number_cwd):   
		name_array = path.split('/')
		if len(name_array) == 1: return inode_number_cwd
		else:
			child_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(name_array[0], inode_number_cwd)
			if child_inode_number == -1: return -1
			return self.LOOKUP("/".join(name_array[1:]), child_inode_number)

	#PLEASE DO NOT MODIFY
	#MAKES NEW ENTRY OF INODE
	def new_entry(self, path, inode_number_cwd, type):
		if path == '/': #SPECIAL CASE OF INITIALIZING FILE SYSTEM
			interface.new_inode_number(type, inode_number_cwd, "root")
			return True
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)
		parent_inode = interface.INODE_NUMBER_TO_INODE(parent_inode_number) 
		childname = path.split('/')[-1]
		if not parent_inode: return -1
		if childname in parent_inode.directory:
			print("Error FileNameLayer: File already exists!")
			return -1
		child_inode_number = interface.new_inode_number(type, parent_inode_number, childname)  #make new child
		if child_inode_number != -1:
			parent_inode.directory[childname] = child_inode_number
			interface.update_inode_table(parent_inode, parent_inode_number)







# MAKE SURE THAT ALL THESE FUNCTIONS ARE UTILIZING LOOKUP CORRECTLY
# MAKE SURE THAT YOU ARE USING FILE NAMES VS FILE PATHS CORRECTLY AS WELL


	#IMPLEMENTS READ
	def read(self, path, inode_number_cwd, offset, length):
		'''WRITE YOUR CODE HERE'''

		# Split filename and filepath
		filepath, filename = os.path.split(path)

		# Find the file's parent's inode number
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)

		# Find the file's inode number
		file_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(filename, parent_inode_number)

		# Call the InodeNumberLayer read function
		retData = interface.read(file_inode_number, offset, length, parent_inode_number)

		if retData == -1:
			print "\nError: issue in reading data from file at inode_number " + file_inode_number + ".\n"
			return -1
		else:
			return 0

	
	#IMPLEMENTS WRITE
	def write(self, path, inode_number_cwd, offset, data):
		'''WRITE YOUR CODE HERE'''

		# Split filename and filepath
		filepath, filename = os.path.split(path)

		# Find the file's parent's inode number
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)

		# Find the file's inode number
		file_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(filename, parent_inode_number)

		# Call the InodeNumberLayer read function
		retData = interface.write(file_inode_number, offset, data, parent_inode_number)

		if retData == -1:
			print "\nError: issue in writing data to file at inode_number " + file_inode_number + ".\n"
			return -1
		else:
			return 0


	#HARDLINK
	def link(self, old_path, new_path, inode_number_cwd):
		'''WRITE YOUR CODE HERE'''

		# Split paths and names for usage
		new_link_path, new_link_name = os.path.split(new_path)
		child_path, child_name = os.path.split(old_path)

		# Find the parent to the child inode number
		# MAKE SURE HOW THIS IS SUPPOSED TO WORK FOR SEARCHING OUTSIDE OF CURRENT WORKING DIRECTORY
		parent_inode_number = self.LOOKUP(old_path, inode_number_cwd)
		new_link_parent_inode_number = self.LOOKUP(new_path, inode_number_cwd)

		# Find the child inode number
		child_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(child_name, parent_inode_number)

		# Call InodeNumberLayer link with parent inode number for new path
		linkErr = interface.link(child_inode_number, new_link_name, new_link_parent_inode_number)

		if linkErr == -1:
			print "\nError: issue in linking " + new_path + " to " + old_path + "\n"
			return -1
		else:
			return 0




	#REMOVES THE FILE/DIRECTORY
	def unlink(self, path, inode_number_cwd):
		if path == "": 
			print("Error FileNameLayer: Cannot delete root directory!")
			return -1
		'''WRITE YOUR CODE HERE'''

		# Find the childpath and childname by splitting the path string
		# MAKE SURE IMPORTING THE OS LIBRARY IS OKAY
		childpath, childname = os.path.split(path)

		# Get child inode number from the inode_number_cwd
		child_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(childname, inode_number_cwd)

		# Call the InodeNumberLayer unlink
		unlinkErr = interface.unlink(child_inode_number, inode_number_cwd, childname)

		# Return something?
		if unlinkErr == -1:
			print "\nError: issue in unlinking " + path + "\n"
			return -1
		else:
			return 0

	#MOVE
	def mv(self, old_path, new_path, inode_number_cwd):
		'''WRITE YOUR CODE HERE'''

		# Create link at the new path
		linkErr = self.link(old_path, new_path, inode_number_cwd)

		# Delete link at the old path
		unlinkErr = self.unlink(old_path, inode_number_cwd)

		# Return something?
		if (linkErr == -1) or (unlinkErr == -1):
			print "\nError: issue in moving file from " + old_path + " to " + new_path + ".\n"
			return -1
		else:
			return 0


	