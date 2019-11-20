# Linux Simple Character Driver

This is a very simple linux character device that has some basic functions that will be explained below.
This code has learning purposes. If you are interested in learning how to develop a *Linux Device Driver*, you can use it as a reference. I tried to write it as simple as possible to make it easy to understand. I hope you can use it to learn something.

# How it works

The device has a very simple functionality: Give to user the following answers: *FIRST!*, *SECOND!*, *THIRD!* according to what user has written to it. The device accepts the following inputs: "*1*", "*2*" or "*3*". So, if you write "1" on the device, and read it after that, you will receive the following answer: "*FIRST!*".
The device just check the first byte of a value written to it, so if you write "*2adfadfadfa*" on the device, the "*2*" value will be captured by the device and, after that, if you read the device you will obtain the following string: "*SECOND!*"

# Loading and unloading the driver

In this repository there is a script called *init_char_device*. If you execute this device with super user permissions, you will have your module loaded to the kernel and the device node "*/dev/chardevice0*" created on the filesystem.
To unload the module, there is another script: *release_char_device*. Executing this script will unload your driver from the kernel and delete the device nodes created before as well.

# Debugging

If you have learning purposed, I advise you to debug the device. The code is full of *KERN_DEBUG* prints, and debugging you can see these prints and understand better how character driver works. To make this, you have to enable the *KERNEL DEBUG LOG LEVEL*. This can be done in many ways. One of these ways is executting the following line with super user permission:

```console
$ echo 7 > /proc/sys/kernel/printk
```

After that, you will be able to see the Driver Debug Messages on the kernel buffer. To read the kernel buffer, one of the options is using unix command *dmesg*:

```console
$ dmesg
```

# Sample: Using the device

Following are a sequence of steps that I did to init, write, read and debug the device *chardevice0* on a linux terminal.

```console
$ cd repository_folder
$ make

*Output:*
make -C /lib/modules/4.12.14-lp151.28.20-default/build SUBDIRS=/home/repository_folder modules
make[1]: Entering directory '/usr/src/linux-4.12.14-lp151.28.20-obj/x86_64/default'
  CC [M]  /home/repository_folder/char_device.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/repository_folder/char_device.mod.o
  LD [M]  /home/repository_folder/char_device.ko
make[1]: Leaving directory '/usr/src/linux-4.12.14-lp151.28.20-obj/x86_64/default'

$ sudo ./init_char_device
$ dmesg

*Output:*
[33560.614940] Value assigned to "first": FIRST!
[33560.614941] Value assigned to "second": SECOND!
[33560.614941] Value assigned to "third": THIRD!
[33560.614942] Value assigned to "not defined": NOT DEFINED!
[33560.614942] Value assigned to "user choice": 0

$ echo "1" > /dev/chardevice0
$ head -c 10 /dev/chardevice0 && echo 

*Output:*
FIRST!

$ echo "2" > /dev/chardevice0
$ head -c 10 /dev/chardevice0 && echo 

*Output:*
SECOND!

$ echo "3" > /dev/chardevice0
$ head -c 10 /dev/chardevice0 && echo 

*Output:*
THIRD!

$ echo "anything except 1,2 or 3" > /dev/chardevice0
$ head -c 15 /dev/chardevice0 && echo 

*Output:*
NOT DEFINED!

$ sudo ./release_char_device

*Output:*
[34007.162644] Value assigned to "first": FIRST!
[34007.162645] Value assigned to "second": SECOND!
[34007.162646] Value assigned to "third": THIRD!
[34007.162646] Value assigned to "not defined": NOT DEFINED!
[34007.162647] Value assigned to "user choice": 0
[34014.434355] Opening device 0...
[34014.434375] Copying the value 1 from user to kernel...
[34014.434378] Releasing the device 0...
[34019.319451] Opening device 0...
[34019.319456] Copying the value FIRST! from kernel to user...
[34019.319459] Releasing the device 0...
[34023.118632] Opening device 0...
[34023.118644] Copying the value 2 from user to kernel...
[34023.118646] Releasing the device 0...
[34024.423423] Opening device 0...
[34024.423429] Copying the value SECOND! from kernel to user...
[34024.423433] Releasing the device 0...
[34027.850661] Opening device 0...
[34027.850684] Copying the value 3 from user to kernel...
[34027.850686] Releasing the device 0...
[34030.059333] Opening device 0...
[34030.059338] Copying the value THIRD! from kernel to user...
[34030.059341] Releasing the device 0...
[34036.842525] Opening device 0...
[34036.842536] Copying the value a from user to kernel...
[34036.842538] Releasing the device 0...
[34040.815307] Opening device 0...
[34040.815313] Copying the value NOT DEFINED! from kernel to user...
[34040.815316] Releasing the device 0...
[34046.658518] Removing device chardevice0...
```
