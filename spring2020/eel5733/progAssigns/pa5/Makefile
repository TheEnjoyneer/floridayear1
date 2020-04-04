obj-m += asp_mycdev.o

all:
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) modules

clean:
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) clean

test:
	gcc -o asp_mycdev_test asp_mycdev_test.c
