// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 09");

static ssize_t mymounts_read(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	struct dentry	*root_dentry, *child_dentry;
	struct path	root;
	size_t		tmplen;
	char		*data;
	int		res;

	data = kmalloc(PAGE_SIZE, GFP_KERNEL);
	get_fs_root(init_task.fs, &root);
	root_dentry = root.mnt->mnt_root;
	snprintf(data, PAGE_SIZE, "root\t%s\n", root_dentry->d_name.name);
	list_for_each_entry(child_dentry, &root_dentry->d_subdirs, d_child) {
		tmplen = strlen(data);
		if (child_dentry->d_flags & DCACHE_MOUNTED)
			snprintf(data + tmplen, PAGE_SIZE - tmplen, "%s\t%s%s\n", child_dentry->d_name.name, child_dentry->d_parent->d_name.name, child_dentry->d_name.name);
	}
	res = simple_read_from_buffer(buf, len, offset, data, strlen(data));
	kfree(data);
	return res;
}

static struct proc_ops mymounts_fops = {
	.proc_read = &mymounts_read
};

static int __init hello_init(void)
{
	if (!proc_create("mymounts", 0444, NULL, &mymounts_fops))
		return -ENOMEM;
	return 0;
}

static void __exit hello_cleanup(void)
{
	remove_proc_entry("mymounts", NULL);
}

module_init(hello_init);
module_exit(hello_cleanup);
