obj-m += mydriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod mydriver.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
stop:
	sudo rmmod mydriver.ko
