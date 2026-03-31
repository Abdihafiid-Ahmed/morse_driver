#Name of the final module: morse_mod.ko
obj-m := morse_mod.o

#Point to the object files in the 'implementation' subdirectory
morse_mod-objs := implementation/morse_dev.o \
                  implementation/morse_led.o \
                  implementation/morse_encode.o \
                  implementation/morse_buffer.o \
                  implementation/morse_thread.o

#where to find your .h files
ccflags-y := -I$(src)/Headers

# Kernel build directory
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf implementation/*.o implementation/.*.cmd
