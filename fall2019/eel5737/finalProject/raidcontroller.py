import xmlrpclib, math, config, functools, hashlib, time, pickle

portNum = 8000
maxDataBlocks = ((config.INODE_SIZE - 63 - config.MAX_FILE_NAME_SIZE) / 2) * (config.NUM_OF_SERVERS - 1)

class virtBlock():

    def __init__(self, block_number):
        N = config.NUM_OF_SERVERS
        self.virt_block_number = block_number
        self.serverNum = block_number % N
        self.serverBlock = -1
        self.virtParityBlock = (N * (block_number / N)) + ((N - 1) - ((block_number / N) % N))
        self.valid = -1


class raidController():

    def __init__(self):
        # Server state array to keep
        self.serverStates = [True] * config.NUM_OF_SERVERS
        # Initialize server proxies
        self.proxy = []
        for i in range(config.NUM_OF_SERVERS):
            self.proxy.append(xmlrpclib.ServerProxy("http://localhost:" + str(portNum + i) + "/"))
        # Initialize a vNode table
        self.vNodeTable = [False] * (config.MAX_NUM_INODES * (config.NUM_OF_SERVERS - 1))
        # VirtBlocks split into (num_of_servers - 1) blocks
        # and have the parity for that data stored in a block on a server as well
        self.vBlockTable = [""] * maxDataBlocks
        for i in range(maxDataBlocks):
            self.vBlockTable[i] = virtBlock(i)


    # CLIENT REQUEST TO INITIALIZE THE MEMORY SYSTEM
    def Initialize(self):
        for i in range(config.NUM_OF_SERVERS):
            try:
                serverRetVal, self.serverStates[i] = pickle.loads(self.proxy[i].Initialize())
                if self.getNumServerFailures() > 1:
                    print("Error: Too many failed servers, Quitting now.\n")
                    quit()

            except xmlrpclib.Error as err:
                # print error message
                print "Error initializing the filesystem on server number:" + str(portNum + i)
                print "Fault code: %d" % err.faultCode
                print "Fault string: %s" % err.faultString
                self.serverStates[i] = False
                if self.getNumServerFailures() > 1:
                    print("Error: Too many failed servers, Quitting now.\n")
                    quit()


    # Return number of server failures
    def getNumServerFailures(self):
        numServerFailures = 0
        for i in range(config.NUM_OF_SERVERS):
            if self.serverStates[i] == False:
                numServerFailures += 1
        return numServerFailures


    # # Locate party
    # def locate_parity(self,address):
    #     N = config.NUM_OF_SERVERS
    #     parity = (N * (address / N)) + ((N - 1) - ((address / N) % N))
    #     return parity


    # Given a virtual block number, go retrieve the data for just that block
    # This function does not handle checks on data, it only runs the server read request
    def get_valid_virt_block(self, block_number):
        try:
            server = self.vBlockTable[block_number].serverNum
            # Validate the block here as well
            self.vBlockTable[block_number].valid = 1
            if self.serverStates[server] == False:
                # If a -2 is in the value of a block on a server
                # Then we know that block must be computed from
                # parity and other disk blocks
                return -2

            retVal, self.serverStates[server] = pickle.loads(self.proxy[server].get_valid_data_block())
            # Always print server failures if they exist
            if self.serverStates[server] == False:
                print("Warning: Server #" + str(server) + " has failed.\n")

            return retVal

        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.get_valid_virt_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            self.serverStates[self.vBlockTable[block_number].serverNum] = False
            if self.getNumServerFailures() > 1:
                print("Error: Too many failed servers, Quitting now.\n")
                quit()


    # Free an individual server's data block
    def free_virt_block(self, block_number):
        time.sleep(config.DELAY_LENGTH)
        try:
            server = self.vBlockTable[block_number].serverNum
            block = self.vBlockTable[block_number].serverBlock
            self.vBlockTable[block_number].valid = -1
            if self.serverStates[server] == True:
                # Make sure this works correctly
                retVal, self.serverStates[server] = pickle.loads(self.proxy[server].free_data_block(pickle.dumps(block)))
                # Always print server failures if they exist
                if self.serverStates[server] == False:
                    print("Warning: Server #" + str(server) + " has failed.\n")
                    return retVal
            else:
                return -1
        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.free_virt_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            self.serverStates[self.vBlockTable[block_number].serverNum] = False
            if self.getNumServerFailures() > 1:
                print("Error: Too many failed servers, Quitting now.\n")
                quit()

    # Given a block number and data string calculate a checksum value
    # Append the checksum to the string and write it to a server
    def update_virt_block(self, block_number, block_data):
        try:
            # Calculate checksum from block_data
            new_block_data = self.data_to_checksum(block_data)
            server = self.vBlockTable[block_number].serverNum
            retVal, self.serverStates[server] = pickle.loads(self.proxy[server].update_data_block(pickle.dumps(new_block_data)))
            if self.serverStates[server] == False:
                print("Warning: Server #" + str(server) + " has failed.\n")

            return retVal

        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.update_virt_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            self.serverStates[self.vBlockTable[block_number].serverNum] = False
            if self.getNumServerFailures() > 1:
                print("Error: Too many failed servers, Quitting now.\n")
                quit()


    # This function is the lowest level function that will actually just run
    # the request to get a data block from a server
    # This function assumes the server has not failed
    def get_virt_data_block(self, block_number):
        try:
            server = self.vBlockTable[block_number].serverNum
            block = self.vBlockTable[block_number].serverBlock
            retVal, self.serverStates[server] = pickle.loads(self.proxy[server].get_data_block(pickle.dumps(block)))
            # Always print server failures if they exist
            if self.serverStates[server] == False:
                print("Warning: Server #" + str(server) + " has failed.\n")

            return retVal
        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.get_virt_data_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            self.serverStates[self.vBlockTable[block_number].serverNum] = False
            if self.getNumServerFailures() > 1:
                print("Error: Too many failed servers, Quitting now.\n")
                quit()


    # Returns data that is re-created from the other disks/parity disk
    def get_fixed_data_block(self, block_number, failed):
        # Loop through and create list of blocks to read from to recreate
        # the data from a block number that is in a failed server
        recoveryBlocks = []
        recoveryData = []
        base_block_number = (block_number / config.NUM_OF_SERVERS) * config.NUM_OF_SERVERS
        for i in range(config.NUM_OF_SERVERS):
            recoveryBlock = base_block_number + i
            if recoveryBlock != block_number:
                recoveryBlocks.append(base_block_number + i)

        # Now request data from each server to operate on
        for i in range(len(recoveryBlocks)):
            recoveryData.append(self.get_virt_data_block(recoveryBlocks[i]))

        blockData = [] * len(recoveryData)
        # Check the checksum of each individual block
        # And save the data to blockData
        for i in range(len(recoveryData)):
            blockData[i] = self.checksum_to_data(recoveryData[i])
            if blockData[i] == "Checksum_Failed" and failed == True:
                blockData[i] == self.get_fixed_data_block(recoveryBlocks[i], False)
            elif blockData[i] == "Checksum_Failed" and failed == False:
                print("Fatal Error: Trying to recreate data from a non-failed server while another server has failed.")
                quit()

        # Recreate the data
        # Determines parity of data
        fixedData = functools.reduce((lambda x,y: x^y), blockData)

        # Return the data
        return fixedData


    # Calculate checksum and append data string with it
    def data_to_checksum(self, block_data):
        checksum = hashlib.md5()
        checksum.update(block_data)
        checksumString = str(checksum.hexdigest().decode("hex"))
        new_block_data = block_data + checksumString
        return new_block_data

    def checksum_to_data(self, block_data):
        # Runs checksum check, if it is bad, returns a -1
        print block_data
        if block_data == None:
            return ""
        old_block_data = block_data[:-16]
        old_checksum = block_data[-16:]
        checksum = hashlib.md5()
        checksum.update(old_block_data)
        new_checksum = str(checksum.hexdigest().decode("hex"))
        if old_checksum != new_checksum:
            return "Checksum_Failed"
        else:
            return old_block_data


    #REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
    def inode_number_to_inode(self, inode_number):
        return self.vNodeTable[inode_number]


    #REQUEST THE DATA FROM THE SERVER
    def get_data_block(self, block_number):
        # Find the failed server first
        failedServer = -1
        for i in range(config.NUM_OF_SERVERS):
            if self.serverStates[i] == False:
                failedServer = i
        # Reads data from blocks and concatenates it to return to the caller
        server = self.vBlockTable[block_number].serverNum
        block = self.vBlockTable[block_number].serverBlock

        # If server is failed, then call get_fixed_data_block()
        if server == failedServer:
            blockData = self.get_fixed_data_block(block_number, True)
        else:
            print("Reading from server "), server
            time.sleep(config.DELAY_LENGTH)
            blockData = self.get_virt_data_block(block_number)
            blockData = self.checksum_to_data(blockData)
            if blockData == "Checksum_Failed":
                blockData = self.get_fixed_data_block(block_number, True)

        # return the block data
        return blockData


    #REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER
    def get_valid_data_block(self):
        returnBlock = -1
        for i in range(maxdataBlocks):
            if self.vBlockTable[i].valid == -1:
                # Set returnBlock block number
                returnBlock = self.vBlockTable[i].virt_block_number
                # Check if parity needs to be allocated, and do so if nec.
                parityBlock = self.vBlockTable[i].virtParityBlock
                if self.vBlockTable[parityBlock].valid == -1:
                    self.vBlockTable[parityBlock].serverBlock = self.get_valid_virt_block(parityBlock)
                # Still always get valid block
                self.vBlockTable[returnBlock].serverBlock = self.get_valid_virt_block(returnBlock)
                break

        # Return the data
        return returnBlock


    #REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
    def free_data_block(self, block_number):
        # Possibly do some stuff to check for whether or not to free
        # a parity block if necessary.  Could be difficult so prolly won't do it
        return self.free_virt_block(block_number)


    #REQUEST TO WRITE DATA ON THE THE SERVER
    def update_data_block(self, block_number, block_data):
        # Gather data
        server = self.vBlockTable[block_number].serverNum
        parityBlock = self.vBlockTable[block_number].virtParityBlock

        # Check if server we are writing to has failed
        if self.serverStates[server] == False:
            # Do the recovery version of write
            # vBlocks

            start_location = N*((self.vBlockTable[block_number].virtParityBlock)/N)

            data =[]

            for i in range(start_location,(start_location + N)):
                if(self.vBlockTable[i].virtParityBlock != parityBlock):
                    if(self.vBlocks[i].serverNum != server):
                        data.append(self.get_data_block(i))

            # Determines parity of data
            parity = functools.reduce((lambda x,y: x^y),data)

            print("Writing parity to server "), vBlockTable[parityBlock.virt_block_number].serverNum
            time.sleep(config.DELAY_LENGTH)
            # Update only the parity block
            self.update_virt_block(parityBlock, parity)

        # Do the normal version of write
        else:
            # Read the existing virt_block data
            oldData = self.get_data_block(block_number)
            oldParity = self.get_data_block(parityBlock)
            intData = oldData ^ block_data
            newParity = intData ^ oldParity
            # Update the virtual blocks
            print("Writing data to server number "),server
            time.sleep(config.DELAY_LENGTH)
            self.update_virt_block(block_number, block_data)
            print("Writing parity to server "), vBlockTable[parityBlock.virt_block_number].serverNum
            time.sleep(config.DELAY_LENGTH)
            self.update_virt_block(parityBlock, newParity)


    #REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
    def update_inode_table(self, inode, inode_number):
        self.vNodeTable[inode_number] = inode


    #REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
    def status(self):
        string = ""
        string += "\n\n----------INODE Blocks: ----------\n"
        inode_number = 0
        string += "Inode Table: \n"
        for i in range(len(self.vNodeTable)):
            string += "[" + str(i) + " : " + str(bool(self.vNodeTable[i])) + "]\n"

        string += "\n\n----------DATA Blocks: ----------\n  "
        counter = 0
        for i in range(len(self.vBlockTable)):
            if counter == 25:
                string += "......Showing just part(25) data blocks\n"
                break
            if self.vBlockTable[i].valid == -1:
                blockData = ""
            else:
                blockData = self.get_data_block(self.vBlockTable[i].virt_block_number)
            string += (str(i) + " : " + "".join(blockData)) + "  "
            counter += 1

        string += "\n\n----------HIERARCHY: ------------\n"
        for i in range(len(self.vNodeTable)):
            inode = self.vNodeTable[i]
            if inode:
                if inode.type:
                    string += "\nDIRECTORY: " + inode.name + "\n"
                    for x in inode.blk_numbers: string += "".join(x[:config.MAX_FILE_NAME_SIZE]) + " || "
                    string += "\n"

        # Return the final string
        return string


    # Statistics per server
    def serverStats(self):
        serverReqs = [] * config.NUM_OF_SERVERS
        serverStatString = ""
        for i in range(config.NUM_OF_SERVERS):
            try:
                serverReqs[i], self.serverStates[i] = pickle.loads(self.proxy[i].serverStats())
                serverStatString += "Server " + str(i) + ": completed " + str(serverReqs[i]) + " requests.\n"

            except xmlrpclib.Error as err:
                print "A fault occurred in raidController.get_virt_data_block()"
                print "Fault code: %d" % err.faultCode
                print "Fault string: %s" % err.faultString
                self.serverStates[self.vBlockTable[block_number].serverNum] = False
                if self.getNumServerFailures() > 1:
                    print("Error: Too many failed servers, Quitting now.\n")
                    quit()

        print(serverStatString)
