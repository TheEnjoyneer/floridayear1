import math

N = 4;

class raidcontroller():

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


if __name__ == '__main__':

    test = raidcontroller()
    for i in range(0,27):
        #print test.locate_parity(i)
        #print test.server_number_conversion(i)
        print test.server_address_conversion(i)
