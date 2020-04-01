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

#define MYDEV_NAME "mycdev"
#define ramdisk_size (size_t) (16*PAGE_SIZE)

// struct asp_mycdev {
// 	struct cdev dev;
// 	char *ramdisk;
// 	struct semaphore sem;
// 	int devNo;
// }


static dev_t first;
static unsigned int count = 1;
static int my_major = 700, my_minor = 0;
static struct cdev *asp_mycdev;
static struct class *mycdev_class;

static int mycdrv_open(struct inode *inode, struct file *file)
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

static int mycdev_release(struct inode *inode, struct file *file)
{
	pr_info(" Christopher Brant dictates: CLOSING device: %s:\n\n", MYDEV_NAME);
	return 0;
}

static ssize_t mycdev_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
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

static ssize_t mycdev_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
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

static int __init my_init(void)
{
	// allocate region and cdev
	if (alloc_chrdev_region(&first, 0, count, MYDEV_NAME) < 0)
	{
		pr_err("Failed to allocate custom device region\n");
		return -1;
	}

	if (!(asp_mycdev = cdev_alloc()))
	{
		pr_err("cdev_alloc() failed\n");
		unregister_chr_dev_region(first, count);
		return -1;
	}

	// init and add cdev
	cdev_init(asp_mycdev, &mycdev_fops);
	if (cdev_add(asp_mycdev, first, count) < 0)
	{
		pr_err("cdev_add() failed\n");
		cdev_del(asp_mycdev);
		unregister_chrdev_region(first, count);
		return -1;
	}

	// Create class and dynamically create device node
	mycdev_class = class_create(THIS_MODULE, "mycdev_class");
	device_create(mycdev_class, NULL, first, NULL, "asp_mycdev")

	// Printouts and return
	pr_info("cbrant succeeded in registering character device %s\n", MYDEV_NAME);
	pr_info("MAJOR number = %d, MINOR number = %d\n", MAJOR(first), MINOR(first));

	return 0;
}

static void __exit my_exit(void)
{
	// Destroy or free everything in the reverse order it was created or allocated
	device_destroy(mycdev_class, first);
	class_destroy(mycdev_class);

	if (asp_mycdev)
		cdev_del(asp_mycdev);

	unregister_chrdev_region(first, count);
	pr_info("\ndevice unregistered, and done so on purpose by Christopher Brant\n");
	kfree(ramdisk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Christopher Brant");
MODULE_LICENSE("GPL v2");
