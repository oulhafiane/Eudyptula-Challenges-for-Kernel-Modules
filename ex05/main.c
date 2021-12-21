#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 05");

static int myopen(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Device file fortytwo opened.\n");
	return (0);
}

static int myclose(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Device file fortytwo closed.\n");
	return (0);
}

static ssize_t myread(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	char	login[8] = "zoulhafi";
	int	max = 8;

	if (*offset >= max)
		return (0);
	if (*offset + len > max)
		len = max - *offset;
	if (copy_to_user(buf, login + *offset, len))
		return -EINVAL;
	*offset += len;
	return (len);
}

static ssize_t mywrite(struct file *f, const char __user *buf, size_t len, loff_t *offset)
{
	int	i;
	char	login[8] = "zoulhafi";
	int	max = 8;

	if (len != max)
		return -EINVAL;
	for (i = 0;i < max; i++) {
		if (buf[i] != login[i])
			return -EINVAL;
	}
	return (max);
}

static struct file_operations myfops = {
	.open = myopen,
	.read = myread,
	.write = mywrite,
	.release = myclose
};

static struct miscdevice mydev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.mode = 0666,
	.fops = &myfops
};

static int __init hello_init(void)
{
	int		ret;
	struct device	*dev;

	printk(KERN_INFO "Hello world !\n");
	if ((ret = misc_register(&mydev)) != 0)
		return ret;
	dev = mydev.this_device;
	printk(KERN_INFO "fortytwo misc driver major/minor = (10, %d) registred\n", mydev.minor);
	printk(KERN_INFO "dev node is /dev/%s\n", mydev.name);
	return (0);
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	misc_deregister(&mydev);
}

module_init(hello_init);
module_exit(hello_cleanup);
