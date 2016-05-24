KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := clockddrvr.o

all: clean run 
	@make -s clean

run: load
	dd if=/proc/clockproc count=1;

load: clockddrvr.o
	-su -c "insmod ./clockddrvr.ko; mknod -m 666 /dev/clockddrvr c 33 0;"


clockddrvr.o:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

unload:
	-su -c " rmmod clockddrvr; \
	rm -fr /dev/clockddrvr;"

clean: unload
	-@rm -fr *.o clockddrvr*.o clockddrvr*.ko .clockddrvr*.* clockddrvr*.*.* .tmp_versions [mM]odule*

