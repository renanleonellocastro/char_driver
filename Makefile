# Makefile:  makefile of a simple char driver

# if KERNELRELEASE is not defined, we've been called directly from the command line.
# Invoke the kernel build system.
ifeq (${KERNELRELEASE},)
	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules


clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean


# Otherwise KERNELRELEASE is defined; we've been invoked from the kernel build system and can use its language.
else
	obj-m := char_device.o
endif
