'''
THIS MODULE INTERACTS WITH THE MEMORY
''' 
import time, raidcontroller

#HANDLE FOR MEMORY OPERATIONS
interface = raidcontroller.raidController()


#REQUEST TO BOOT THE FILE SYSTEM
def Initialize_My_FileSystem():
    print("File System Initializing......")
    time.sleep(2)
    state = interface.Initialize()
    if state == False:
        print("File System Could Not Initialize.")
        quit()
    else:
        print("File System Initialized!")


#REQUEST TO FETCH THE INODE FROM INODE NUMBER FROM SERVER
def inode_number_to_inode(inode_number):
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    return interface.inode_number_to_inode(inode_number)


#REQUEST THE DATA FROM THE SERVER
def get_data_block(block_number):
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    return ''.join(interface.get_data_block(block_number))


#REQUESTS THE VALID BLOCK NUMBER FROM THE SERVER 
def get_valid_data_block():
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    return (interface.get_valid_data_block())


#REQUEST TO MAKE BLOCKS RESUABLE AGAIN FROM SERVER
def free_data_block(block_number):
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    interface.free_data_block((block_number))


#REQUEST TO WRITE DATA ON THE THE SERVER
def update_data_block(block_number, block_data):
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    interface.update_data_block(block_number, block_data)


#REQUEST TO UPDATE THE UPDATED INODE IN THE INODE TABLE FROM SERVER
def update_inode_table(inode, inode_number):
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    interface.update_inode_table(inode, inode_number)


#REQUEST FOR THE STATUS OF FILE SYSTEM FROM SERVER
def status():
    if (interface.getNumServerFailures() > 1):
        print("Intolerable number of servers have failed. Quitting now.\n")
        quit()
    return interface.status()
