/*
 * char_dev.c -- Very simple character device
 * Description: A simple device that, on read, return FIRST!, SECOND! or THIRD!
 *  according to user's write (1, 2 or 3). Example: If user write "1" to the
 *  device, and then user read the device, the device will answer "FIRST!".
 *  This device is used and created for learning purposes.
 *
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

#include "char_device.h"

//Open function: Executed when user call the syscall to open the device
//---------------------------------------------------------------------------------------------------------------------
int char_device_open(struct inode *char_device_inode, struct file *filep)
{
    struct char_dev_data *char_device_data;
    unsigned int device_minor_number;
    char_device_data = container_of(char_device_inode->i_cdev, struct char_dev_data, cdev);
    filep->private_data = char_device_data; // here we save the data for other methods to use it
    device_minor_number = MINOR(char_device_inode->i_rdev);
    printk(KERN_DEBUG "Opening device %u...\n", device_minor_number);
    return 0;
}

//Close function: Executed when user call the syscall to close the device
//---------------------------------------------------------------------------------------------------------------------
int char_device_release(struct inode *char_device_inode, struct file *filep)
{
    struct char_dev_data *char_device_data;
    unsigned int device_minor_number;
    char_device_data = container_of(char_device_inode->i_cdev, struct char_dev_data, cdev);
    device_minor_number = MINOR(char_device_inode->i_rdev);
    printk(KERN_DEBUG "Releasing the device %u...\n", device_minor_number);
    return 0;
}

//Read function: Executed when user call the syscall to read the device
//---------------------------------------------------------------------------------------------------------------------
ssize_t char_device_read(struct file *filep, char __user *user_buffer, size_t length, loff_t *offset)
{
    struct char_dev_data *char_device_data = filep->private_data;
    ssize_t ret = 0;
    ssize_t size_of_transfered_data = 0;

    if (down_interruptible(&char_device_data->sem)) {
        return -ERESTARTSYS;
    }

    switch(char_device_data->user_choice) {

        case '1':
            ret = copy_to_user(user_buffer, &char_device_data->first_answer, sizeof(char_device_data->first_answer));
            size_of_transfered_data = sizeof(char_device_data->first_answer);
        break;

        case '2':
            ret = copy_to_user(user_buffer, &char_device_data->second_answer, sizeof(char_device_data->second_answer));
            size_of_transfered_data = sizeof(char_device_data->second_answer);
        break;

        case '3':
            ret = copy_to_user(user_buffer, &char_device_data->third_answer, sizeof(char_device_data->third_answer));
            size_of_transfered_data = sizeof(char_device_data->third_answer);
        break;

        default:
            ret = copy_to_user(user_buffer, &char_device_data->not_defined, sizeof(char_device_data->not_defined));
            size_of_transfered_data = sizeof(char_device_data->not_defined);
    }

    if (ret > 0) {
       ret = -EFAULT;
       up(&char_device_data->sem);
       return ret;
    }

    printk(KERN_DEBUG "Copying the value %s from kernel to user...\n", &user_buffer[0]);
    (*offset)+=size_of_transfered_data;
    ret = size_of_transfered_data;
    up(&char_device_data->sem);
    return ret; //return to user the size of the data that was written
}

//Write function: Executed when user call the syscall to write data to the device
//---------------------------------------------------------------------------------------------------------------------
ssize_t char_device_write(struct file *filep, const char __user *user_buffer, size_t length, loff_t *offset)
{
    //this code just analyse the first byte that user wrote
    struct char_dev_data *char_device_data = filep->private_data;
    ssize_t ret = 0;

    if (down_interruptible(&char_device_data->sem))
        return -ERESTARTSYS;

    ret = copy_from_user(&char_device_data->user_choice, user_buffer, 1);

    if (ret > 0) {
        ret = -EFAULT;
        up(&char_device_data->sem);
        return ret;
    }

    printk(KERN_DEBUG "Copying the value %c from user to kernel...\n", char_device_data->user_choice);
    (*offset)+=length;
    ret = (ssize_t) length;
    up(&char_device_data->sem);
    return ret; //just analyse the first byte but tell user that all was read
}

//Cleanup function: Executed when user call the syscall to remove the module from kernel(rmmod)
//---------------------------------------------------------------------------------------------------------------------
void char_device_exit(void)
{
    uint8_t i;
    dev_t device_number = (dev_t) MKDEV(major_number, minor_number);

    if (char_devices_data) { //remove char devices
        for (i = 0; i < number_of_devices; ++i) {
            printk(KERN_DEBUG "Removing device chardevice%d...\n", i);
            cdev_del(&char_devices_data[i].cdev);
        }
        kfree(char_devices_data);
    }

    unregister_chrdev_region(device_number, number_of_devices);
}

//Setup function: Used by init to setup the kernel cdev struct
//---------------------------------------------------------------------------------------------------------------------
static void char_device_configure(struct char_dev_data *char_device_data, int char_device_index)
{
    int error; //used to verify errors while trying to add the device number to kernel cdev struct
    int device_number;
    char first[10] = "FIRST!";
    char second[10] = "SECOND!";
    char third[10] = "THIRD!";
    char not_defined[15] = "NOT DEFINED!";

    //setup the values of the answers
    strncpy(char_device_data->first_answer, first, sizeof(char_device_data->first_answer));
    strncpy(char_device_data->second_answer, second, sizeof(char_device_data->second_answer));
    strncpy(char_device_data->third_answer, third, sizeof(char_device_data->third_answer));
    strncpy(char_device_data->not_defined, not_defined, sizeof(char_device_data->not_defined));
    char_device_data->user_choice = '0';

    //print debug level
    printk(KERN_DEBUG "Value assigned to \"first\": %s\n",&char_device_data->first_answer[0]);
    printk(KERN_DEBUG "Value assigned to \"second\": %s\n",&char_device_data->second_answer[0]);
    printk(KERN_DEBUG "Value assigned to \"third\": %s\n",&char_device_data->third_answer[0]);
    printk(KERN_DEBUG "Value assigned to \"not defined\": %s\n",&char_device_data->not_defined[0]);
    printk(KERN_DEBUG "Value assigned to \"user choice\": %c\n", char_device_data->user_choice);

    //get the device number from major and minor numbers
    device_number = MKDEV(major_number, minor_number + char_device_index);

    //initialize the cdev struct from kernel with the file operations struct
    cdev_init(&char_device_data->cdev, &char_device_fops);

    //allocate the owner of the cdev struct as this module
    char_device_data->cdev.owner = THIS_MODULE;

    //allocate the operations of the cdev struct as char_device_fops(this module file operations)
    char_device_data->cdev.ops = &char_device_fops;

    //add the device number to the cdev struct and verify if some error ocurred
    error = cdev_add(&char_device_data->cdev, device_number, 1);

    //warn in error case
    if (error)
        printk(KERN_NOTICE "Error: %d while adding chardevice%d", error, char_device_index);
}

//Init function: Executed when user call the syscall to add the module from kernel(insmod)
//---------------------------------------------------------------------------------------------------------------------
int char_device_init(void)
{
    int error;
    uint8_t i;
    dev_t device_number = 0;

    if (major_number) { //static allocation for major number if it is defined
        device_number = (dev_t) MKDEV(major_number, minor_number);
        error = register_chrdev_region(device_number, number_of_devices, "chardevice");
    } else { //dynamic allocation for major number if it is not defined
        error = alloc_chrdev_region(&device_number, minor_number, number_of_devices, "chardevice");
        major_number = (uint8_t) MAJOR(device_number);
    }

    if (error < 0) {
        printk(KERN_WARNING "chardevice: Problems getting major number %d\n", major_number);
        return error;
    }

    //allocating the devices data
    char_devices_data = kmalloc(number_of_devices * sizeof(struct char_dev_data), GFP_KERNEL);

    //checking for errors in allocation
    if (!char_devices_data) {
        error = -ENOMEM;
        char_device_exit();
        return error;
    }

    //set the devies memory data block with 0 in all bytes
    memset(char_devices_data, 0, number_of_devices * sizeof(struct char_dev_data));

   //initialize all devices
    for (i = 0; i < number_of_devices; ++i) {
      sema_init(&char_devices_data[i].sem, 1);
      char_device_configure(&char_devices_data[i], i);
   }

   return 0;
}

//---------------------------------------------------------------------------------------------------------------------

