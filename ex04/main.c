#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 04");

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello world !\n");
	return (0);
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
