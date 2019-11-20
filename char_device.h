/*
 * char_device.h -- definitions for the char module

 * GPL LICENSE 3
 * Copyright (c) 2019 Renan L. Castro renanleonellocastro@gmail.com

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

//Cheking definition of the header file
//---------------------------------------------------------------------------------------------------------------------
#ifndef _CHAR_DEVICE_H_
#define _CHAR_DEVICE_H_

//Kernel headers that will be necessary for the device creation
//---------------------------------------------------------------------------------------------------------------------
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

//Defining major number if developer want to
//---------------------------------------------------------------------------------------------------------------------
#ifndef MAJOR_NUMBER
#define MAJOR_NUMBER 0
#endif

//Defining the number of devices
//---------------------------------------------------------------------------------------------------------------------
#ifndef NUMBER_OF_DEVICES
#define NUMBER_OF_DEVICES 1 //chardevice0
#endif

//Char device data struct. Used in all device operations to change data with user
//---------------------------------------------------------------------------------------------------------------------
struct char_dev_data {
    char first_answer[10];
    char second_answer[10];
    char third_answer[10];
    char not_defined[15];
    char user_choice;
    struct semaphore sem; //mutual exclusion semaphore (handle simultaneous operations)
    struct cdev cdev; //char device (from kernel) structure
};

//Some global stuff here.
//---------------------------------------------------------------------------------------------------------------------
uint8_t major_number =   MAJOR_NUMBER;
uint8_t minor_number =   0;
uint8_t number_of_devices = NUMBER_OF_DEVICES;
struct char_dev_data *char_devices_data;

//Prototypes of this device functions
//---------------------------------------------------------------------------------------------------------------------
int char_device_open(struct inode*, struct file*);
int char_device_release(struct inode*, struct file*);
ssize_t char_device_read(struct file*, char __user*, size_t, loff_t*);
ssize_t char_device_write(struct file*, const char __user*, size_t, loff_t*);
int char_device_init(void);
void char_device_exit(void);
static void char_device_configure(struct char_dev_data*, int);

//File operations stuff (kernel stuff that is necessary to work with drivers)
//---------------------------------------------------------------------------------------------------------------------
struct file_operations char_device_fops = {
   .owner =    THIS_MODULE,
   .read =     char_device_read,
   .write =    char_device_write,
   .open =     char_device_open,
   .release =  char_device_release,
};

//Telling the kernel about author, license, init and exit function
//---------------------------------------------------------------------------------------------------------------------
MODULE_AUTHOR("Renan Leonello Castro");
MODULE_LICENSE("GPL v3");
module_init(char_device_init);
module_exit(char_device_exit);
//---------------------------------------------------------------------------------------------------------------------

#endif
