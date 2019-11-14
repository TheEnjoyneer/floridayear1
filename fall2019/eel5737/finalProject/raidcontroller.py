import math, config

portNum = 8000

class raidController():
    # Total number of blocks in our virtual memory is equal to the number of blocks per server
    self.maxDataBlocks = (config.INODE_SIZE - 63 - config.MAX_FILE_NAME_SIZE) / 2
    # Virtual data block size is equal to the size of an individual server's data block * (num_servers - 1)
    self.virtBlockSize = config.BLOCK_SIZE * (config.NUM_OF_SERVERS - 1)



    def __init__(self):
        # Initialize the memory map array and the server proxies
        # COME BACK HERE AND DECIDE ON WHETHER OR NOT WE HAVE NUMBER DATA BLOCKS FOR ALL SERVERS OR N-1 SERVER
        # Initialize server proxies
        for i in range(config.NUM_OF_SERVERS):
            self.proxy.append(xmlrpclib.ServerProxy("http://localhost:" + str(portNum + i) + "/"))


    def locate_parity(self,address):
        parity = (N-1) - ((address/(N-1)) % N)
        return parity

    def server_number_conversion(self,address):
        if((address % (N-1)) < self.locate_parity(address)):
            return (address %(N-1))
        else:
            return ((address %(N-1)) + 1)

    def server_address_conversion(self,address):
        return address/(N-1)

    # CLIENT REQUEST TO INITIALIZE THE MEMORY SYSTEM
    def Initialize(self):
        for i in range(config.NUM_OF_SERVERS):
            try:
                self.proxy[i].Initialize()
            except Exception as err:
                # print error message
                    print "Error in re-initializing the filesystem on server number:" + str(portNum + i)
                    quit()


    #REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
    def inode_number_to_inode(self, inode_number):
        # Find the inode_number given and translate it to the server number and inode number for that server
        # Request that inode from that server using that inode number
        inode_number = inode_number % config.MAX_NUM_INODES
        inode_server = inode_number / config.MAX_NUM_INODES
        try:
            return pickle.loads(self.proxy[inode_server].inode_number_to_inode(pickle.dumps(inode_number)))
        except xmlrpclib.Error as err:
            print "A fault occurred in raidController.inode_number_to_inode()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUEST THE DATA FROM THE SERVER
    def get_data_block(self, block_number):
        # Find the block number given and translate it to the server number and block number for that server
        # Request that block data from the server using that block number
        try:
            return pickle.loads(self.proxy.get_data_block(pickle.dumps(block_number)))
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.get_data_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
    def get_valid_data_block(self):
        # Return the correct data
        try:
            return pickle.loads(self.proxy.get_valid_data_block())
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.get_valid_data_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
    def free_data_block(self, block_number):
        # Return the possible error, if no error, set retErr to 0
        try:
            return pickle.loads(self.proxy.free_data_block(pickle.dumps(block_number)))
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.free_data_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUEST TO WRITE DATA ON THE THE SERVER
    def update_data_block(self, block_number, block_data):
        # Return the possible error, if no error, set retErr to 0
        try:
            return pickle.loads(self.proxy.update_data_block(pickle.dumps(block_number), pickle.dumps(block_data)))
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.update_data_block()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
    def update_inode_table(self, inode, inode_number):
        # Find the inode_number given and translate it to the server number and inode number for that server
        # Request that inode from that server using that inode number
        inode_number = inode_number % config.MAX_NUM_INODES
        inode_server = inode_number / config.MAX_NUM_INODES
        try:
            return pickle.loads(self.proxy[inode_server].update_inode_table(pickle.dumps(inode), pickle.dumps(inode_number)))
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.update_inode_table()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


    #REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
    def status(self):
        # Return the status string after marshalling the data
        try:
            return pickle.loads(self.proxy.status())
        except xmlrpclib.Error as err:
            print "A fault occurred in client_stub.status()"
            print "Fault code: %d" % err.faultCode
            print "Fault string: %s" % err.faultString
            quit()


# Main function for testing purposes
if __name__ == '__main__':

    test = raidController()
    for i in range(0,27):
        #print test.locate_parity(i)
        #print test.server_number_conversion(i)
        print test.server_address_conversion(i)
