import BlockLayer, sys, config, MemoryInterface

MemoryInterface.Initialize_My_FileSystem()
interface = BlockLayer.BlockLayer()


class Operations():
	def __init__(self):
		self.map = []

	#WRITES STRING1
	def write(self, string):
		data_array = []
		# verify that string is of type string
		for i in range(0, len(string), config.BLOCK_SIZE):
			# divide up the string into chunks of length BLOCK_SIZE
			data_array.append(string[i : i + config.BLOCK_SIZE])
		self.__write_to_filesystem(data_array)


	#READS THE STRING
	def read(self):
		data = []
		for i in range(len(self.map)):
			# index through block numbers in map to get data blocks
			data.append(interface.BLOCK_NUMBER_TO_DATA_BLOCK(self.map[i]))
		print( "".join(data))
		return "".join(data)

	#WRITE TO FILESYSTEM
	def __write_to_filesystem(self, data_array):
		for i in range(len(data_array)):
			valid_block_number = interface.get_valid_data_block()
			interface.update_data_block(valid_block_number, data_array[i])
			self.map.append(valid_block_number)

	#STATUS FUNCTION TO CHECK THE STATUS OF THE DATA BLOCKS IN THE MEMORY
	def status(self):
		print(MemoryInterface.status())

	# WRITE TO OFFSET (refer to assignment doc)
	def write_to_offset(self,offset,string):	
		''' WRITE CODE HERE '''
		offset_block = (offset / config.BLOCK_SIZE) + self.map[0]
		prev_end_block = self.map[-1]
		offset_slot = offset % config.BLOCK_SIZE
		# if offset is too large, return error and end execution
		if offset_block > prev_end_block:
			print "\nError: Given offset is out of necessary bounds.\n"
			exit()
		else:
			# loop through the string and update blocks
			data_to_block = ""
			for i in range(0, len(string)):
				block_contents = interface.BLOCK_NUMBER_TO_DATA_BLOCK(offset_block)
				# if we are within the prev string section of blocks
				if offset_block <= prev_end_block:
					# loop to add to a block
					if data_to_block == "":
							data_to_block = block_contents[0:offset_slot]
					if offset_slot in range(0, config.BLOCK_SIZE - 1):
						data_to_block += string[i]
						offset_slot += 1
					else:
						data_to_block += string[i]
						offset_slot = 0
						interface.update_data_block(offset_block, data_to_block)
						if offset_block == prev_end_block:
							offset_block = interface.get_valid_data_block()
							self.map.append(offset_block)
						else:
							offset_block += 1
						data_to_block = ""
				# if we are outside of the inital section of blocks
				else:
					if offset_slot in range(0, config.BLOCK_SIZE - 1):
						data_to_block += string[i]
						#print data_to_block
						#print offset_block
						#print offset_slot
						offset_slot += 1
					else:
						data_to_block += string[i]
						offset_slot = 0
						interface.update_data_block(offset_block, data_to_block)
						offset_block = interface.get_valid_data_block()
						self.map.append(offset_block)
						data_to_block = ""
			# Update the last of the blocks if it doesn't finish the block
			interface.update_data_block(offset_block, data_to_block)


if __name__ == "__main__":
	if len(sys.argv) < 3: 
		print("Usage: python HW1.py <string1> <string2>")
		exit(0)
	test = Operations()
	test.write(sys.argv[1])
	test.read()
	test.status()
	test.write_to_offset(int(sys.argv[3]),sys.argv[2])
	test.read()
	# last call 
	test.status()

