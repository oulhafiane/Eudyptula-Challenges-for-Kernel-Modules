// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 04");

static struct usb_device_id keyboard_table[] = {
	{
		USB_INTERFACE_INFO
		(
			USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD
		)
	}
};

static struct usb_driver keyboard_driver = {
	.name = "mykeyboard_driver_assignment_04",
	.id_table = keyboard_table
};

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello world from usb driver!\n");
	return usb_register(&keyboard_driver);
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up usb driver module.\n");
	usb_deregister(&keyboard_driver);
}

module_init(hello_init);
module_exit(hello_cleanup);
