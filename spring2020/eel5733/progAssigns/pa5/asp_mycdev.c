/* asp_mycdev.c
 * Christopher Brant
 * Programming Assignment 5
 * EEL 5733 Advanced Systems Programming
 * University of Florida
 * 4/4/20
 */

#include <linux/module.h>	/* for modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev utilities */

#define MYDRV_NAME "asp_mycdev"
#define MYDEV_NAME "/dev/mycdev"
//#define ramdisk_size (size_t) (16*PAGE_SIZE)

struct asp_mycdev {
	struct cdev dev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
	size_t ramdisk_size = (size_t) (16*PAGE_SIZE);
}

// Parameters to be set at load time
static int mycdev_major;
static int mycdev_minor = 0;
static int mycdev_nr_devs = 3;

module_param(mycdev_major, int, S_IRUGO);
module_param(mycdev_minor, int, S_IRUGO);
module_param(mycdev_nr_devs, int, S_IRUGO);

// More necessary global variables
static dev_t devNum;
static unsigned int count = 1;
static struct class *mycdev_class;
static struct asp_mycdev *mycdevices;


static int mycdev_open(struct inode *inode, struct file *file)
{
	// static int for counting how many times the open func has executed
	static int openCount = 0;

	// Set the dev structure up first so we are doing the right thing
	struct asp_mycdev mycdev;
	mycdev = container_of(inode->i_cdev, struct asp_mycdev, cdev);
	file->private_data = mycdev;

	// Printouts
	pr_info("cbrant attempting to open device: %s\n", MYDEV_NAME);
	pr_info("MAJOR number = %d, MINOR number = %d\n", imajor(inode), iminor(inode));

	// Increment counter
	openCount++;

	// Then print messages
	pr_info("cbrant succesfully opened device: %s:\n\n", MYDEV_NAME);
	pr_info("I have been opened %d times since being loaded.\n", openCount);
	pr_info("ref=%d\n", (int)module_refcount(THIS_MODULE));

	return 0;
}



// FIRST SLIDESET
static int mycdev_release(struct inode *inode, struct file *file)
{

	pr_info(" Christopher Brant dictates: CLOSING device: %s:\n\n", MYDEV_NAME);
	return 0;
}


// FIRST SLIDESET
static ssize_t mycdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	// Declare necessary variables
	int nbytes;

	// Set the dev structure up first so we are doing the right thing
	struct asp_mycdev mycdev = file->private_data;

	// Synchronization primitives...
	if (down_interruptible(&mycdev->sem))
		return -ERESTARTSYS;

	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("Christopher Brant dictates: trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_to_user(buf, (mycdev->ramdisk) + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("\n Christopher Brant dictates: READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);

	// Synchronization primitives...
	up(&mycdev->sem);

	return nbytes;
}


// FIRST SLIDESET
static ssize_t mycdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	// Declare necessary variables
	int nbytes;

	// Set the dev structure up first so we are doing the right thing
	struct asp_mycdev dev = file->private_data;

	// Synchronization primitives...
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("Christopher Brant dictates: trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user((mycdev->ramdisk) + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("\n Christopher Brant dictates: WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);

	// Synchronization primitives...
	up(&dev->sem);

	return nbytes;
}


// FIRST SLIDESET
static loff_t mycdev_lseek(struct file *file, loff_t * offset, int orig)
{
	// Declare necessary variables
	loff_t newpos;
	char *newdisk;
	char *tempdisk;

	// Set the dev structure up first so we are doing the right thing
	struct asp_mycdev mycdev = file->private_data;

	// Synchronization primitives...
	if (down_interruptible(&mycdev->sem))
		return -ERESTARTSYS;

	// Switch statement to test where to pur the cursor
	switch (orig)
	{
		case SEEK_SET:
			newpos = offset;
			break;

		case SEEK_CUR:
			newpos = file->f_pos + offset;
			break;

		case SEEK_END:
			newpos = mycdev->ramdisk_size + offset;
			break;

		default:
			return -EINVAL;
	}

	// Test for errors
	newpos = newpos >= 0 ? newpos : 0;

	if (newpos > mycdev->ramdisk_size)
	{
		// Allocate new disk
		newdisk = kmalloc(newpos, GFP_KERNEL);

		// Copy over old data
		memcpy(newdisk, mycdev->ramdisk, mycdev->ramdisk_size);

		// Extend new data to the length of the position
		memset((newdisk + mycdev->ramdisk_size), '\0', (newpos - mycdev->ramdisk_size));

		// Switch newdisk and original ramdisk and then free the original ramdisk
		tempdisk = mycdev->ramdisk;

		mycdev->ramdisk = newdisk;

		kfree(tempdisk);
	}



	// If newpos is greater than the size of the buffer...
	// Expand the buffer and fill the new region with zeros.



	// Set new cursor position, print, and then return
	file->f_pos = newpos;
	pr_info("Seeking to cursor position=%ld\n", (long)newpos);

	// Synchronization primitives...
	up(&mycdev->sem);

	return newpos;
}

static int mycdev_ioctl(struct inode *inode, struct file *file, unsigned int command, unsigned long args)
{
	// Declare necessary variables
	int err = 0;
	int retVal = 0;
	int tmp;

	// Set the dev structure up first so we are doing the right thing
	struct asp_mycdev mycdev = file->private_data;


	// HERE SHOULD GO ALL THE SCULL IOCTL MAGIC BS ON PAGE 21


	// Synchronization primitives...
	if (down_interruptible(&mycdev->sem))
		return -ERESTARTSYS;

	// Switch statement for handling the control commands
	switch (cmd)
	{
		// Clear the buffer and reset the file position pointer to 0
		case ASP_CLEAR_BUF:

	}


	return retVal;
}

static const struct file_operations mycdev_fops = {
	.owner = THIS_MODULE,
	.read = mycdev_read,
	.write = mycdev_write,
	.open = mycdev_open,
	.release = mycdev_release,
	.lseek = mycdev_lseek,
	.ioctl = mycdev_ioctl,
};

// FIRST SLIDESET
static int __init my_init(void)
{
	// Declare necessary variables
	char devName[13];
	int i;
	dev_t currDev;

	// Allocate all devices
	mycdevices = kmalloc(mycdev_nr_devs * sizeof(struct asp_mycdev), GFP_KERNEL);

	// allocate region and cdev
	if (alloc_chrdev_region(&devNum, mycdev_minor, mycdev_nr_devs, MYDRV_NAME) < 0)
	{
		pr_err("Failed to allocate custom device region for %s, for %d devices\n", MYDRV_NAME, mycdev_nr_devs);
		return -1;
	}

	// Set the major number correctly if not already done
	mycdev_major = MAJOR(devNum);

	// Create class and dynamically create device node
	mycdev_class = class_create(THIS_MODULE, "mycdev_class");


	// For loop to create the individual devices.
	for (i = 0; i < mycdev_nr_devs; i++)
	{
		// Get the current device number to give to the device creation function
		currDev = MKDEV(mycdev_major, i);

		// This should work as we have to allocate the cdev structures
		if (!(mycdevices[i].dev = cdev_alloc()))
		{
			pr_err("cdev_alloc() failed for index %d\n", i);
			unregister_chrdev_region(devNum, mycdev_nr_devs);
			return -1;
		}

		// Set device name string
		sprintf(devName, "%s%d", MYDEV_NAME, i);

		// Init and add cdev the individual cdev structs within my structure
		currDev = MKDEV(mycdev_major)
		cdev_init(&(mycdevices[i].dev), &mycdev_fops);

		// Add the cdev, but with some error checks
		if (cdev_add(&(mycdevices[i].dev), currDev, 1))
		{
			printk(KERN_NOTICE "Error %d adding %s\n", retVal, devName);
			return -1;
		}

		// Create the device
		device_create(mycdev_class, NULL, devNum, NULL, devName);

		// Allocate and set all the structure variables here too
		mycdevices[i].ramdisk = kmalloc(mycdevices[i].ramdisk_size, GFP_KERNEL);
		mycdevices[i].devNo = currDev;
		init_MUTEX(&mycdevices[i].sem); 

		// Printouts and return
		pr_info("cbrant succeeded in registering character device %s\n", devName);
		pr_info("MAJOR number = %d, MINOR number = %d\n", MAJOR(devNum), MINOR(devNum));

	}

	return 0;
}


static void __exit my_exit(void)
{
	int i;
	int major = MAJOR(devNum);
	dev_t currDev;

	// Loop through devices and delete the cdevs then destroy the devices
	for (i = 0; i < mycdev_nr_devs; i++)
	{
		currDev = MKDEV(mycdev_major, i);
		cdev_del(&mycdevices[i].dev);
		device_destroy(mycdev_class, currDev);

		// Also free the individual asp_mycdev structure's ramdisk
		kfree(mycdevices[i].ramdisk);
	}

	// Free the custom struct array destroy the class, unregister the region and print
	class_destroy(mycdev_class);
	unregister_chrdev_region(devNum, mycdev_nr_devs);
	kfree(mycdevices);
	pr_info("\ndevice unregistered, and done so on purpose by Christopher Brant\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Christopher Brant");
MODULE_LICENSE("GPL v2");
