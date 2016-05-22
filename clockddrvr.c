#include <linux/version.h>
#include <generated/utsrelease.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/memory.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

#include "clockddrvr.h"

MODULE_LICENSE("GPL");

static struct proc_dir_entry* j_file;

static int j_show(struct seq_file *m, void *v)
{

	seq_printf(m, "Hello World\n" );

	return 0;
}

static int j_open(struct inode *inode, struct file *file)
{
     return single_open(file, j_show, NULL);
}

static const struct file_operations j_fops = {
     .owner	= THIS_MODULE,
     .open	= j_open,
     .read	= seq_read,
     .llseek	= seq_lseek,
     .release	= single_release,
};


static int __init t_init(void) {
	printk(KERN_ALERT "INIT" );

	j_file = proc_create("clockproc", 0, NULL, &j_fops);
	if (!j_file) {
		return -ENOMEM;
	}

	return 0;
}

static void __exit t_exit(void) {

	remove_proc_entry("clockproc", NULL);

	printk(KERN_ALERT "DONE" );

}

module_init(t_init);
module_exit(t_exit);
