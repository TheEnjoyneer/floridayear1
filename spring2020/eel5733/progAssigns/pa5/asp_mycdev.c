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
#define MYDEV_NAME "mycdev"
#define ramdisk_size (size_t) (16*PAGE_SIZE)

struct asp_mycdev {
	struct cdev dev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
}

// Parameters to be set at load time
int mycdev_major;
int mycdev_minor = 0;
int mycdev_nr_devs;
// int mycdev_quantum = MYCDEV_QUANTUM;
// int mycdev_qset = MYCDEV_QSET;

module_param(mycdev_major, int, S_IRUGO);
module_param(mycdev_minor, int, S_IRUGO);
module_param(mycdev_nr_devs, int, S_IRUGO);
// module_param(mycdev_quantum, int, S_IRUGO);
// module_param(mycdev_qset, int, S_IRUGO);

// More necessary global variables
static dev_t first;
static unsigned int count = 1;

// FIX THIS IF NEED BE
static struct asp_mycdev *cdb_devices;
static struct class *mycdev_class;

// DON'T KNOW IF I NEED THIS
static int my_major = 700, my_minor = 0;


// THIS IS FROM THE FIRST SLIDESET
static int mycdev_open(struct inode *inode, struct file *file)
{
	// static int for counting how many times the open func has executed
	static int openCount = 0;

	// Allocate a new ramdisk for each open
	char *ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
	file->private_data = ramdisk;
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


// THIS IS FROM THE SCULL SLIDESET
static int mycdev_open(struct inode *inode, struct file *file)
{
	// Get device info and container_of
	struct asp_mycdev *dev = container_of(inode->i_cdev, struct asp_mycdev, cdev);
	file->private_data = dev;

	// If write only trim length of the device

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
	// Immediately set ramdisk equal to file's private data
	char *ramdisk = file->private_data;

	int nbytes;
	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("Christopher Brant dictates: trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_to_user(buf, ramdisk + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("\n Christopher Brant dictates: READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}


// FIRST SLIDESET
static ssize_t mycdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	// Immediately set ramdisk equal to file's private data
	char *ramdisk = file->private_data;
	int nbytes;
	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("Christopher Brant dictates: trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user(ramdisk + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("\n Christopher Brant dictates: WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}


// FIRST SLIDESET
static loff_t mycdev_lseek(struct file *file, loff_t * offset, int orig)
{
	// Immediately set ramdisk equal to file's private data
	char *ramdisk = file->private_data;

	// Declare loff_t for testing purposes
	loff_t newpos;

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
			newpos = ramdisk_size + offset;
			break;

		default:
			return -EINVAL;
	}

	// Test for errors
	newpos = newpos < ramdisk_sizse ? newpos : ramdisk_size;
	newpos = newpos >= 0 ? newpos : 0;

	// Set new cursor position, print, and then return
	file->f_pos = newpos;
	pr_info("Seeking to cursor position=%ld\n", (long)newpos);

	return newpos;
}

static int mycdev_ioctl(struct inode *inode, struct file *file, unsigned int command, unsigned long args)
{
	// Immediately set ramdisk equal to file's private data
	char *ramdisk = file->private_data;

	int nbytes;
	

	// IOCTL function here


	return nbytes;
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

// // FIRST SLIDESET
// static int __init my_init(void)
// {
// 	// allocate region and cdev
// 	if (alloc_chrdev_region(&first, 0, count, MYDEV_NAME) < 0)
// 	{
// 		pr_err("Failed to allocate custom device region\n");
// 		return -1;
// 	}

// 	if (!(asp_mycdev = cdev_alloc()))
// 	{
// 		pr_err("cdev_alloc() failed\n");
// 		unregister_chr_dev_region(first, count);
// 		return -1;
// 	}

// 	// init and add cdev
// 	cdev_init(asp_mycdev, &mycdev_fops);
// 	if (cdev_add(asp_mycdev, first, count) < 0)
// 	{
// 		pr_err("cdev_add() failed\n");
// 		cdev_del(asp_mycdev);
// 		unregister_chrdev_region(first, count);
// 		return -1;
// 	}

// 	// Create class and dynamically create device node
// 	mycdev_class = class_create(THIS_MODULE, "mycdev_class");
// 	device_create(mycdev_class, NULL, first, NULL, "asp_mycdev");

// 	// Printouts and return
// 	pr_info("cbrant succeeded in registering character device %s\n", MYDEV_NAME);
// 	pr_info("MAJOR number = %d, MINOR number = %d\n", MAJOR(first), MINOR(first));

// 	return 0;
// }


// SCULL SLIDESET
static int __init my_init(void)
{
	int result, i;
	dev_t dev = 0;

	// Get dynamic major number
	if (alloc_chrdev_region(&first, 0, count, MYDEV_NAME) < 0)
	{
		pr_err("Failed to allocate custom device region\n");
		return -1;
	}

	
	

	// Get a range of minor numbers to work with



	// Make the device node itself here
	if (mycdev_major)
	{
		dev = MKDEV(mycdev_major, mycdev_minor);
		result = register_chrdev_region(dev, scull_nr_devs, MYDRV_NAME);
	}
	else
	{
		result = alloc_chrdev_region(&dev, mycdev_minor, mycdev_nr_devs, MYDRV_NAME);
		mycdev_major = MAJOR(dev);
	}

	// Error checking here
	if (result < 0)
	{
		printk(KERN_WARNING "asp_mycdev: can't get major %d\n", mycdev_major);
		return result;
	}

	// Allocate the devices here, and the number can be specified at load time
	cdb_devices = kmalloc(mycdev_nr_devs * sizeof(struct asp_mycdev), GFP_KERNEL);

	// Error check
	if (!cdb_devices)
	{
		result = -ENOMEM;
		return result;	// this may have to change
	}

	// Set the memory of the devices
	memset(cdb_devices, 0, mycdev_nr_devs * sizeof(struct asp_mycdev));

	// Initialize each individual devices
	for (i = 0; i < mycdev_nr_devs; i++)
	{
		cdb_devices[i].devNo = i;
		init_MUTEX(&cdb_devices[i].sem);
		setup_mycdev(&cdb_devices[i], i);
	}

	// If we succeed to here, return 0
	return 0;
}

static void setup_mycdev(struct asp_mycdev *mycdev, int index)
{
	int err;
	int devno = MKDEV(mycdev_major, mycdev_minor + index);

	cdev_init(&mycdev->dev, mycdev_fops);
	mycdev->dev.owner = THIS_MODULE;
	mycdev->dev.ops = &mycdev_fops;
	err = cdev_add(&mycdev->dev, devno, 1);

	// Error checking
	if (err)
		printk(KERN_NOTICE "Error %d adding mycdev%d", err, index);
}

// // FIRST SLIDESET
// static void __exit my_exit(void)
// {
// 	// Destroy or free everything in the reverse order it was created or allocated
// 	device_destroy(mycdev_class, first);
// 	class_destroy(mycdev_class);

// 	if (asp_mycdev)
// 		cdev_del(asp_mycdev);

// 	unregister_chrdev_region(first, count);
// 	pr_info("\ndevice unregistered, and done so on purpose by Christopher Brant\n");
// 	kfree(ramdisk);
// }

// SCULL SLIDESET
static void __exit my_exit(void)
{
	int i;
	dev_t devno = MKDEV(mycdev_major, mycdev_minor);

	// Remove all character device entires
	if (cdb_devices)
	{
		for (i = 0; i < mycdev_nr_devs; i++)
		{
			scull_trim(cdb_devices + i);
			cdev_del(&cdb_devices[i].dev);
		}

		// Free the device array
		kfree(cdb_devices);
	}

	// Unregister device region
	unregister_chrdev_region(devno, mycdev_nr_devs);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Christopher Brant");
MODULE_LICENSE("GPL v2");
