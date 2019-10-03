# HW3 Initial Test File

import InodeLayer, config, datetime, InodeOps, InodeNumberLayer, 

# Initialize the filesystem
MemoryInterface.Initialize_My_FileSystem()

#HANDLE OF INODE LAYER
interface = InodeNumberLayer.InodeNumberLayer()

# Begin Test conditions here
inode1_number = interface.new_inode_number(1, -1, "/")
inode2_number = interface.new_inode_number()













