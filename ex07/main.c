#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 07");

DEFINE_MUTEX(mtx);
static struct dentry	*fortytwo;
static char		data[PAGE_SIZE];

static ssize_t id_read(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	char	login[8] = "zoulhafi";
	int	max = 8;

	if (*offset >= max)
		return (0);
	if (*offset + len > max)
		len = max - *offset;
	if (copy_to_user(buf, login + *offset, len))
		return (-EINVAL);
	*offset += len;
	return (len);
}

static ssize_t id_write(struct file *f, const char __user *buf, size_t len, loff_t *offset)
{
	int	i;
	char	login[8] = "zoulhafi";
	char	tmp[8];
	int	max = 8;

	if (len != max)
		return (-EINVAL);
	if (copy_from_user(tmp, buf, max))
		return (-EINVAL);
	for (i = 0;i < max; i++) {
		if (tmp[i] != login[i])
			return (-EINVAL);
	}
	return (max);
}

static const struct file_operations id_fops = {
	.read = id_read,
	.write = id_write
};

static ssize_t jiffies_read(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	int	n;
	char	localbuf[50];

	if (*offset != 0)
		return (0);
	if (len > 50)
		len = 50;
	memset(localbuf, 0, 50);
	n = snprintf(localbuf, len, "%ld", jiffies);
	return (simple_read_from_buffer(buf, len, offset, localbuf, strlen(localbuf)));
}

static const struct file_operations jiffies_fops = {
	.read = jiffies_read
};

static ssize_t foo_read(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	int status = 0;
	mutex_lock(&mtx);
	if (*offset >= PAGE_SIZE)
		goto unlock_mtx_rd;
	if (len + *offset > PAGE_SIZE)	
		len = PAGE_SIZE - *offset;
	if (copy_to_user(buf, data + *offset, len)) {
		status = -EINVAL;
		goto unlock_mtx_rd;
	}
	*offset += len;
	status = len;
unlock_mtx_rd:
	mutex_unlock(&mtx);
	return (status);
}

static ssize_t foo_write(struct file *f, const char __user *buf, size_t len, loff_t *offset)
{
	int status = 0;
	mutex_lock(&mtx);
	if (*offset >= PAGE_SIZE)
		goto unlock_mtx_wr;
	if (len + *offset > PAGE_SIZE)
		len = PAGE_SIZE - *offset;
	if (copy_from_user(data + *offset, buf, len)) {
		status = -EINVAL;
		goto unlock_mtx_wr;
	}
	*offset += len;
	status = len;
unlock_mtx_wr:
	mutex_unlock(&mtx);
	return (status);
}

static const struct file_operations foo_fops = {
	.read = foo_read,
	.write = foo_write
};

static int __init hello_init(void)
{
	struct dentry *id, *jiffies, *foo;

	if (!IS_ENABLED(CONFIG_DEBUG_FS))
		return (-EINVAL);
	printk(KERN_INFO "Hello world from our debugfs module!\n");
	fortytwo = debugfs_create_dir("fortytwo", NULL);
	if (!fortytwo)
		return (PTR_ERR(fortytwo));
	id = debugfs_create_file("id", 0666, fortytwo, NULL, &id_fops);
	if (!id) {
		debugfs_remove_recursive(fortytwo);
		return (PTR_ERR(id));
	}
	jiffies = debugfs_create_file("jiffies", 0444, fortytwo, NULL, &jiffies_fops);
	if (!jiffies) {
		debugfs_remove_recursive(fortytwo);
		return (PTR_ERR(id));
	}
	foo = debugfs_create_file("foo", 0644, fortytwo, NULL, &foo_fops);
	if (!foo) {
		debugfs_remove_recursive(fortytwo);
		return (PTR_ERR(id));
	}
	memset(data, 0, PAGE_SIZE);
	printk(KERN_INFO "/sys/kernel/debug/fortytwo created successfully.\n");
	printk(KERN_INFO "/sys/kernel/debug/fortytwo/id created successfully.\n");
	printk(KERN_INFO "/sys/kernel/debug/fortytwo/jiffies created successfully.\n");
	printk(KERN_INFO "/sys/kernel/debug/fortytwo/foo created successfully.\n");
	return (0);
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up our debugfs module!\n");
	debugfs_remove_recursive(fortytwo);
}

module_init(hello_init);
module_exit(hello_cleanup);
