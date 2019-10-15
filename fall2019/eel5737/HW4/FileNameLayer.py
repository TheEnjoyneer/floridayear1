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


	#IMPLEMENTS READ
	def read(self, path, inode_number_cwd, offset, length):

		# Split filename and filepath
		filepath, filename = os.path.split(path)

		# Find the file's parent's inode number
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)

		# Find the file's inode number
		file_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(filename, parent_inode_number)

		# If parent_inode_number or file_inode_number are bad, return error
		if (parent_inode_number == -1) or (file_inode_number == False):
			print "\nError: FileNameLayer LOOKUP failed to find file to read from.\n"
			return -1
		else:
			# Call the InodeNumberLayer read function
			retData = interface.read(file_inode_number, offset, length, parent_inode_number)

			if retData == -1:
				print "\nError: issue in reading data from file at inode_number ", file_inode_number, "\n"
				return -1
			else:
				return retData

	
	#IMPLEMENTS WRITE
	def write(self, path, inode_number_cwd, offset, data):

		# Split filename and filepath
		filepath, filename = os.path.split(path)

		# Find the file's parent's inode number
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)
		# Find the file's inode number
		file_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(filename, parent_inode_number)

		# If parent_inode_number or file_inode_number are bad, return error
		if (parent_inode_number == -1) or (file_inode_number == False):
			print "\nError: FileNameLayer LOOKUP failed to find file to write to.\n"
			return -1
		else:
			# Call the InodeNumberLayer read function
			retErr = interface.write(file_inode_number, offset, data, parent_inode_number)

			if retErr == -1:
				print "\nError: issue in writing data to file at inode_number ", file_inode_number, "\n"
				return -1
			else:
				return True


	#HARDLINK
	def link(self, old_path, new_path, inode_number_cwd):

		# Split paths and names for usage
		new_link_path, new_link_name = os.path.split(new_path)
		child_path, child_name = os.path.split(old_path)

		# Find the parent to the child inode number
		parent_inode_number = self.LOOKUP(old_path, inode_number_cwd)
		new_link_grandparent_inode_number = self.LOOKUP(new_path, inode_number_cwd)

		if (parent_inode_number == -1) or (new_link_grandparent_inode_number == -1):
			print "\nError: One or more parent inode numbers are invalid for linking in FileNameLayer.\n"
			return -1

		# Get inode number at next location
		new_link_parent_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(new_link_name, new_link_grandparent_inode_number)
		# Find the child inode number
		child_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(child_name, parent_inode_number)
		child_inode = interface.INODE_NUMBER_TO_INODE(child_inode_number)

		if (child_inode_number == False) or (new_link_parent_inode_number == False):
			print "\nError: issue in finding child inode number to create new hard link for.\n"
			return -1
		else:
			# Call InodeNumberLayer link with parent inode number for new path
			new_link_parent_inode = interface.INODE_NUMBER_TO_INODE(new_link_parent_inode_number)
			child_inode = interface.INODE_NUMBER_TO_INODE(child_inode_number)

			# This is here for the ambiguity in how somebody may give arguments to the move function
			if new_link_parent_inode == False:
				new_link_parent_inode_number = new_link_grandparent_inode_number
				if child_inode.type != 1 and new_link_name != "":
					child_name = new_link_name
			else:
				print "\nWarning: Attempted to rename a directory upon moving it."
				print "Our system does not allow this as we save directory names in the inode."
				print "And this becomes a complex issue of how to know when to rename the inode within"
				print "the framework we are working in.\n"

			if child_name == "":
				print "\nWarning: Invalid name to link to: [blank name].  Moving with same name."
				return -1

			linkErr = interface.link(child_inode_number, child_name, new_link_parent_inode_number)

			if linkErr == -1:
				print "\nError: issue in linking " + new_path + " to " + old_path + "\n"
				return -1
			else:
				return True


	#REMOVES THE FILE/DIRECTORY
	def unlink(self, path, inode_number_cwd):
		if path == "": 
			print("Error FileNameLayer: Cannot delete root directory!")
			return -1

		# Find the childpath and childname by splitting the path string
		# MAKE SURE IMPORTING THE OS LIBRARY IS OKAY
		childpath, childname = os.path.split(path)

		# Get the parent_inode_number
		parent_inode_number = self.LOOKUP(path, inode_number_cwd)

		if parent_inode_number == -1:
			print "\nError: FileNameLayer LOOKUP failed to find file to unlink.\n"
			return -1

		# Get child inode number from the inode_number_cwd
		child_inode_number = self.CHILD_INODE_NUMBER_FROM_PARENT_INODE_NUMBER(childname, parent_inode_number)

		# If file_inode_number is bad, return error
		if child_inode_number == False:
			print "\nError: FileNameLayer unlink failed to find the child inode number.\n"
			return -1
		else:
			# Call the InodeNumberLayer unlink
			unlinkErr = interface.unlink(child_inode_number, parent_inode_number, childname)

			# Return something?
			if unlinkErr == -1:
				print "\nError: issue in unlinking " + path + "\n"
				return -1
			else:
				return True

	#MOVE
	def mv(self, old_path, new_path, inode_number_cwd):
		
		# Link to the new path
		linkErr = self.link(old_path, new_path, inode_number_cwd)

		if linkErr == -1:
			print "\nError: issue in moving file from " + old_path + " to " + new_path + ".\n"
			return -1
		else:
			# Delete link at the old path
			unlinkErr = self.unlink(old_path, inode_number_cwd)

			if unlinkErr == -1:
				print "\nError: issue in moving file from " + old_path + " to " + new_path + ".\n"
				return -1
			else:
				return True


	