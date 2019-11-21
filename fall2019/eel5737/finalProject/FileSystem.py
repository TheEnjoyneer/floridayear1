import MemoryInterface, AbsolutePathNameLayer, time
from cmd import Cmd

def Initialize_My_FileSystem():
    MemoryInterface.Initialize_My_FileSystem()
    #AbsolutePathNameLayer.AbsolutePathNameLayer().new_entry('/', 1)

#HANDLE TO ABSOLUTE PATH NAME LAYER
interface = AbsolutePathNameLayer.AbsolutePathNameLayer()

class MyPrompt(Cmd):
    prompt = '$ '
    intro = 'Welcome to the RAID-5 Filesystem by Nick Poindexter and Christopher Brant'

    def do_create(self, inp):
        if inp == "":
            print "Bad Syntax, Nice Try Cody!!"
            return
        my_object.create(inp)

    def do_rm(self, inp):
        if inp == "":
            print "Bad Syntax, Nice Try Cody!!"
            return
        my_object.rm(inp)

    def do_mv(self,inp):
        if inp == "":
            print "Bad Syntax, Nice Try Cody!!"
            return
        input = inp.split()
        if len(input) > 2:
            print "Bad Syntax, Nice Try Cody!!"
            return
        my_object.mv(input[0],input[1])

    def do_mkdir(self,inp):
        if inp == "":
            print "Bad Syntax, Nice Try Cody!!"
            return
        my_object.mkdir(inp)

    def do_status(self,inp):
        my_object.status()

    def do_write(self,inp):
        inp = inp.replace('"', '')
        input = inp.split()
        my_object.write(input[0],inp[len(input[0]+1):(-(1+len(input[-1])))],int(input[-1]))

    def do_read(self,inp):
        if inp == "":
            print "Bad Syntax, Nice Try Cody!!"
            return
        input = inp.split()
        return_data = my_object.read(input[0],int(input[1]),int(input[2]))

    def do_exit(self, inp):
        print("Why are you leaving me just like my father did?")
        return True


class FileSystemOperations():

    #MAKES NEW DIRECTORY
    def mkdir(self, path):
        interface.new_entry(path, 1)

    #CREATE FILE
    def create(self, path):
        interface.new_entry(path, 0)


    #WRITE TO FILE
    def write(self, path, data, offset=0):
        interface.write(path, offset, data)


    #READ
    def read(self, path, offset=0, size=-1):
        read_buffer = interface.read(path, offset, size)
        if read_buffer != -1: print(path + " : " + read_buffer)


    #DELETE
    def rm(self, path):
        interface.unlink(path)


    #MOVING FILE
    def mv(self, old_path, new_path):
        interface.mv(old_path, new_path)


    #CHECK STATUS
    def status(self):
        print(MemoryInterface.status())

if __name__ == '__main__':
    #DO NOT MODIFY THIS

    Initialize_My_FileSystem()
    AbsolutePathNameLayer.AbsolutePathNameLayer().new_entry('/', 1)
    my_object = FileSystemOperations()
    #my_object.status()
    #YOU MAY WRITE YOUR CODE AFTER HERE

    #timing_start = time.time()

    #print "Welcome to the redundant RAID-5 Filesystem by Nick Poindexter and Christopher Brant"


    MyPrompt().cmdloop()


    # my_object.mkdir("/A")
    # my_object.status()
    # my_object.mkdir("/B")
    # my_object.status()
    # my_object.create("/A/1.txt")#, as A is already there we can create file in A
    # my_object.status()
    # my_object.write("/A/1.txt", "POCSD", 0)#, as 1.txt is already created now, we can write to it.
    # my_object.read("/A/1.txt", 0, 5)
    # my_object.status()
    # my_object.mv("/A/1.txt", "/B")
    # my_object.status()
    # my_object.rm("/A/1.txt")
    # my_object.status()
    # my_object.read("/B/1.txt", 0, 5)
    # # my_object.rm("/B/1.txt")
    # # my_object.status()
    # # my_object.rm("/B")
    # # my_object.status()
    # # my_object.rm("/A")
    # # my_object.status()


    # my_object.mkdir("/A/C")
    # my_object.status()
    # my_object.mkdir("/D")
    # my_object.status()
    # my_object.mkdir("/E")
    # my_object.status()
    # my_object.read("/B/1.txt", 0, 5)
    # my_object.mv("/B", "/A/C/D")
    # my_object.status()
    # my_object.mv("/A/C/B", "/")
    # my_object.status()
    # my_object.read("/B/1.txt", 0, 10)
    # my_object.mv("/B/1.txt", "/A/C/")
    # my_object.status()

    # timing_end = time.time()

    # timing = timing_end - timing_start

    # print "\nTotal time for execution in nanoseconds is: ", timing, ".\n"


    #my_object.read("/A/C/B/1.txt", 0, 5)


    '''Examples:
    my_object.mkdir("/A")
    my_object.status()
    my_object.mkdir("/B")
    my_object.status()
    my_object.create("/A/1.txt"), as A is already there we can create file in A
    my_object.status()
    my_object.write("A/1.txt", "POCSD", offset), as 1.txt is already created now, we can write to it.
    my_object.status()
    my_object.mv("/A/1.txt", "/B")
    my_object.status()
    my_object.rm("A/1.txt")
    my_object.status()
    '''
