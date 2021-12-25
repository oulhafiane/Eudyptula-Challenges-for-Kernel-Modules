// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/nsproxy.h>
#include <linux/ns_common.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakariaa Oulhafiane <zoulhafi@student.1337.ma>");
MODULE_DESCRIPTION("Assignment 09");

struct mnt_namespace {
	struct ns_common	ns;
	struct mount		*root;
	struct list_head	list;
	spinlock_t		ns_lock;
	struct user_namespace	*user_ns;
	struct ucounts		*ucounts;
	u64			seq;
	wait_queue_head_t poll;
	u64 event;
	unsigned int		mounts;
	unsigned int		pending_mounts;
} __randomize_layout;

struct mount {
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union {
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	/* list of children, anchored here */
	struct list_head mnt_child;	/* and going through their mnt_child */
	struct list_head mnt_instance;	/* mount instance on sb->s_mounts */
	const char *mnt_devname;	/* Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list;
	struct list_head mnt_expire;	/* link in fs-specific expiry list */
	struct list_head mnt_share;	/* circular list of shared mounts */
	struct list_head mnt_slave_list;/* list of slave mounts */
	struct list_head mnt_slave;	/* slave list entry */
	struct mount *mnt_master;	/* slave is on master->mnt_slave_list */
	struct mnt_namespace *mnt_ns;	/* containing namespace */
	struct mountpoint *mnt_mp;	/* where is it mounted */
	union {
		struct hlist_node mnt_mp_list;	/* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;			/* mount identifier */
	int mnt_group_id;		/* peer group identifier */
	int mnt_expiry_mark;		/* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

static ssize_t mymounts_read(struct file *f, char __user *buf, size_t len, loff_t *offset)
{
	struct mount		*child_mount;
	struct dentry		*parent;
	size_t			tmplen;
	char			*data, tmp[20][100];
	const unsigned char	*name;
	int			res, i;

	if (*offset != 0)
		return 0;
	data = kmalloc(PAGE_SIZE, GFP_KERNEL);
	memset(data, 0, PAGE_SIZE);
	tmplen = strlen(data);
	snprintf(data + tmplen, PAGE_SIZE - tmplen, "root\t/\n");
	list_for_each_entry(child_mount, &current->nsproxy->mnt_ns->list, mnt_list) {
		parent = child_mount->mnt_mountpoint;
		if (IS_ROOT(parent))
			continue;
		name = child_mount->mnt_parent->mnt_mountpoint->d_name.name;
		tmplen = strlen(data);
		if (strcmp(name, "/") != 0)
			snprintf(data + tmplen, PAGE_SIZE - tmplen, "%s\t/%s/", child_mount->mnt_mountpoint->d_name.name, name);
		else
			snprintf(data + tmplen, PAGE_SIZE - tmplen, "%s\t/", child_mount->mnt_mountpoint->d_name.name);
		i = -1;
		while (!IS_ROOT(parent) && i <= 20) {
			name = parent->d_parent->d_name.name;
			if (strcmp(name, "/") != 0)
				snprintf(tmp[++i], 100, "%s", name);
			parent = parent->d_parent;
		}
		while (i >= 0) {
			tmplen = strlen(data);
			snprintf(data + tmplen, PAGE_SIZE - tmplen, "%s/", tmp[i--]);
		}
		tmplen = strlen(data);
		snprintf(data + tmplen, PAGE_SIZE - tmplen, "%s\n", child_mount->mnt_mountpoint->d_name.name);
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
