import datetime, config, BlockLayer, InodeOps, MemoryInterface


MemoryInterface.Initialize_My_FileSystem()
#HANDLE OF BLOCK LAYER
interface = BlockLayer.BlockLayer()

class InodeLayer():

    #PLEASE DO NOT MODIFY THIS
    #RETURNS ACTUAL BLOCK NUMBER FROM RESPECTIVE MAPPING  
    def INDEX_TO_BLOCK_NUMBER(self, inode, index):
        if index == len(inode.blk_numbers): return -1
        return inode.blk_numbers[index]


    #PLEASE DO NOT MODIFY THIS
    #RETURNS BLOCK DATA FROM INODE and OFFSET
    def INODE_TO_BLOCK(self, inode, offset):
        index = offset / config.BLOCK_SIZE
        block_number = self.INDEX_TO_BLOCK_NUMBER(inode, index)
        if block_number == -1: return ''
        else: return interface.BLOCK_NUMBER_TO_DATA_BLOCK(block_number)


    #PLEASE DO NOT MODIFY THIS
    #MAKES NEW INODE OBJECT
    def new_inode(self, type):
        return InodeOps.Table_Inode(type)


    #PLEASE DO NOT MODIFY THIS
    #FLUSHES ALL THE BLOCKS OF INODES FROM GIVEN INDEX OF MAPPING ARRAY  
    def free_data_block(self, inode, index):
        for i in range(index, len(inode.blk_numbers)):
            interface.free_data_block(inode.blk_numbers[i])
            inode.blk_numbers[i] = -1


    #IMPLEMENTS WRITE FUNCTIONALITY
    def write(self, inode, offset, data):
        # Calculate maximum file size before doing anything
        maxFileBlocks = (config.INODE_SIZE - 63 - config.MAX_FILE_NAME_SIZE) / 2
        maxFileSize = config.BLOCK_SIZE * maxFileBlocks
        # Error check for if inode is of type: file
        if inode.type != 0:
            print "\nError: inode is not of type: FILE\n"
            exit()
        else:
            # Error check for it offset is greater than file size
            if inode.size < offset:
                print "\nError: offset for write() is greater than file size\n"
                exit()
            else:
                newBlock = 0
                newBlockContents = []
                initBlockIndex = offset / config.BLOCK_SIZE
                offsetBlockIndex = offset / config.BLOCK_SIZE
                offsetByteIndex = offset % config.BLOCK_SIZE
                initBlockNumLength = inode.blk_numbers.length()
                newSize = offset - 1

                # Warn if the length of the write is going to exceed the 
                # maximum file size if all bytes are written
                if (newSize + data.length()) >= maxFileSize:
                    print "\nWarning: Length of data to write at given offset exceeds maximum file size."
                    print "\nProceeding with write call until file is at maximum size......\n"

                # Loop through the data to be written
                # HOW THE FUCK AM I SUPPOSED TO KNOW THE MAX FILE SIZE
                for i in range(data.length()) && newSize < maxFileSize:
                    # Check to see if we are still writing to existing blocks
                    if offsetBlockIndex < initBlockNumLength:
                        currBlock = self.INDEX_TO_BLOCK_NUMBER(inode, offsetBlockIndex)

                        # Check to see if the offset is inside an existing block
                        if (offset % config.BLOCK_SIZE) != 0 && offsetBlockIndex == initBlockIndex:
                            # Retrieve existing contents
                            currBlockContents = interface.BLOCK_NUMBER_TO_DATA_BLOCK(currBlock)
                            # Copy old data into new list up to the place to write
                            # This (offsetByteIndex - 1) is important so that we read up to the 
                            # offsetByteIndex, but not the byte at that index, as we will start writing there
                            for j in range(offsetByteIndex):
                                newBlockContents[j] = currBlockContents[j]

                        # Add the next piece of data and then index offsetByteIndex
                        newBlockContents[offsetByteIndex] = data[i]
                        offsetByteIndex += 1
                        newSize += 1

                        # Check if we need to update offsetBlockIndex
                        if offsetByteIndex == config.BLOCK_SIZE:
                            # Increment offsetBlockIndex and update the now fully changed data block
                            offsetBlockIndex += 1
                            interface.update_data_block(currBlock, newBlockContents)
                            inode.size = newSize
                            # Clear newBlockContents for next writing
                            newBlockContents = []

                    # This runs if we are writing to a location in a new block
                    else:
                        # If this is the first byte index grab a new block.
                        if offsetByteIndex == 0: #&& offsetBlockIndex == initBlockNumLength:
                            if newBlock != 0:
                                # Update the data block
                                interface.update_data_block(newBlock, newBlockContents)
                            
                            # Retrieve a new valid block and add the block number to the inode
                            newBlock = interface.get_valid_data_block()
                            inode.blk_numbers.append(newBlock)
                            inode.size = newSize
                            newBlockContents = []

                        # In any normal case just update the offsetByteIndex and newBlockContents
                        newBlockContents[offsetByteIndex] = data[i]
                        offsetByteIndex += 1
                        newSize += 1

                # if last block is not full, fill with zeros
                while newBlockContents.size() != config.BLOCK_SIZE && newSize < maxFileSize:
                    newBlockContents.append(0)
                    newSize += 1

                # Write the last block to memory
                interface.update_data_block(newBlock, newBlockContents)

                # Update the "accessed" and "modified" times in the inode
                inode.size = newSize
                inode.time_accessed = str(datetime.datetime.now())[:19]
                inode.time_modified = str(datetime.datetime.now())[:19]

                # Return the updated inode
                return inode

    #IMPLEMENTS THE READ FUNCTION 
    def read(self, inode, offset, length): 
        retData = []
        # Error check for if inode is of type: file
        if inode.type != 0:
            print "\nError: inode is not of type: FILE\n"
            exit()
        else:
            # Error check for it offset is greater than file size
            if inode.size < offset:
                print "\nError: offset for read() is greater than file size\n"
                exit()
            else:
                offsetBlockIndex = offset / config.BLOCK_SIZE
                offsetByteIndex = offset % config.BLOCK_SIZE
                offsetByte = offset
                bytesRead = 0

                # Loop through the length of data to be read
                for i in range(length):
                    # Keep reading as long as the offset is within the size of the file
                    if offsetByte < inode.size:
                        offsetByteIndex %= config.BLOCK_SIZE
                        # Check to see if we are still reading from existing blocks
                        if ((offset - 1) + bytesRead) < inode.size:
                            if currBlock != self.INDEX_TO_BLOCK_NUMBER(inode, offsetBlockIndex):
                                currBlock = self.INDEX_TO_BLOCK_NUMBER(inode, offsetBlockIndex)
                                currBlockContents = interface.BLOCK_NUMBER_TO_DATA_BLOCK(currBlock)

                            # Read byte to retData array and then increment bytesRead and offsetByteIndex
                            retData.append(currBlockContents[offsetByteIndex])
                            bytesRead += 1
                            offsetByteIndex += 1
                            offsetByte += 1

                            # Check if we need to update offsetBlockIndex
                            if offsetByteIndex == config.BLOCK_SIZE:
                                offsetBlockIndex += 1
                        else:
                            # Break out of the loop in this case, as it means you are at the end of the file
                            break

                # Update the "accessed" and "modified" times in the inode
                inode.time_accessed = str(datetime.datetime.now())[:19]

                # Return the updated indode and the return data
                return inode, retData

    #IMPLEMENTS THE COPY FUNCTION 
    def copy(self, inode):
        # Error check for if inode is of type: file
        if inode.type != 0:
            print "\nError: inode is not of type: FILE\n"
            exit()
        else:
            inodeCopy = self.new_inode(inode.type)

            for offsetBlockIndex in range(inode.blk_numbers.length()):
                # Retrieve inode block and data
                oldBlock = self.INDEX_TO_BLOCK_NUMBER(inode, offsetBlockIndex)
                oldBlockContents = inteface.BLOCK_NUMBER_TO_DATA_BLOCK(oldBlock)
                # Create inodeCopy valid block and copy data to it
                newBlock = interface.get_valid_data_block()
                newBlockContents = []
                for j in (0, oldBlockContents.length()):
                    newBlockContents.append(oldBlockContents[j])
                # Update newblock contents and append block num to inode
                interface.update_data_block(newBlock, newBlockContents)
                inodeCopy.blk_numbers.append(newBlock)

            # Return the new instance/copied inode
            return inodeCopy

    def status(self):
        print(MemoryInterface.status())



