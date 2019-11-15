import math, config

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


    # ENSURE THAT CODE IS ADDED TO CHECK FOR SERVER FAILURES EVERY TIME A REQUEST IS SENT
    # THIS IS IMPORTANT SO THAT WE HAVE AN ACCURATE REPRESENTATION IN SERVERSTATES AS WELL 
    # AS AN ACCURATE NUMOFSERVERFAILURES SO THAT WE KNOW IF WE SHOULD BE TOLERATING 
    # FAILURE OR JUST QUITTING EXECUTION



    def __init__(self):
        # Server state array to keep 
        self.serverStates = [True] * config.NUM_OF_SERVERS
        self.numServerFailures = 0
        # Initialize server proxies
        for i in range(config.NUM_OF_SERVERS):
            self.proxy.append(xmlrpclib.ServerProxy("http://localhost:" + str(portNum + i) + "/"))
        # Initialize a vNode table
        self.vNodeTable = dict()
        for i in range(config.MAX_NUM_INODES * (config.NUM_OF_SERVERS - 1)):
            self.vNodeTable[i] = False
        # VirtBlocks split into (num_of_servers - 1) blocks
        # and have the parity for that data stored in a block on a server as well
        self.vBlockTable = dict()
        for i in range(maxDataBlocks):
            self.vBlockTable[i] = virtBlock(i)


    # CLIENT REQUEST TO INITIALIZE THE MEMORY SYSTEM
    def Initialize(self):
        for i in range(config.NUM_OF_SERVERS):
            try:
                serverRetVal, self.serverState[i] = self.proxy[i].Initialize()

                # COME BACK AND FINISH THIS
                if (self.serverState[i] == False) and (self.numServerFailures == 0):
                    self.numServerFailures += 1
                else:
                    return 

            except Exception as err:
                # print error message
                    print "Error in re-initializing the filesystem on server number:" + str(portNum + i)
                    quit()


    # Return number of server failures
    def getNumServerFailures(self):
        return self.numServerFailures


    # Locate party
    def locate_parity(self,address):
        N = config.NUM_OF_SERVERS
        parity = (N * (address / N)) + ((N - 1) - ((address / N) % N))
        return parity


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
            return pickle.loads(self.proxy[serverNum].get_valid_data_block())
        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.get_valid_virt_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    # Free an individual server's data block
    def free_virt_block(self, block_number):
        try:
            server = self.vBlockTable[block_number].serverNum
            block = self.vBlockTable[block_number].serverBlock
            self.vBlockTable[block_number].valid = -1
            if self.serverStates[server] == True:
                # Make sure this works correctly
                retVal, self.serverStates[server] = pickle.loads(self.proxy[server].free_data_block(block))
        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.free_virt_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    def update_virt_block(self, block_number):
        try:
            # something
        except xmlrpclib.Error as err:
            # something


    #REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
    def inode_number_to_inode(self, inode_number):
        return self.vNodeTable[inode_number]


    # #REQUEST THE DATA FROM THE SERVER
    # def get_data_block(self, block_number):
    #     # Reads data from blocks and concatenates it to return to the caller
    #     dataString = []



    #     try:

    #     except xmlrpclib.Error as err:
    #         print "A fault occurred in client_stub.get_data_block()"
    #         print "Fault code: %d" % err.faultCode
    #         print "Fault string: %s" % err.faultString
    #         quit()


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
        # Do something
        return self.free_virt_block(block_number)


    #REQUEST TO WRITE DATA ON THE THE SERVER
    def update_data_block(self, block_number, block_data):
        # Gather data
        server = self.vBlockTable[block_number].serverNum
        block = self.vBlockTable[block_number].serverBlock
        parityBlock = self.vBlockTable[block_number].virtParityBlock
        
        # Check if server we are writing to has failed
        if self.serverStates[server] == False:
            # Do the recovery version of write


            
        # Do the normal version of write
        else:


    #REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
    def update_inode_table(self, inode, inode_number):
        self.vNodeTable[inode_number] = inode


    # #REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
    # def status(self):
    #     # Return the status string after marshalling the data
    #     try:
    #         return pickle.loads(self.proxy.status())
    #     except xmlrpclib.Error as err:
    #         print "A fault occurred in client_stub.status()"
    #         print "Fault code: %d" % err.faultCode
    #         print "Fault string: %s" % err.faultString
    #         quit()


# Main function for testing purposes
if __name__ == '__main__':

    test = raidController()
    for i in range(0,27):
        #print test.locate_parity(i)
        #print test.server_number_conversion(i)
        print test.server_address_conversion(i)
