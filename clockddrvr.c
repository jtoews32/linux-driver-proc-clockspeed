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
#include <linux/time.h>
#include <asm/msr.h>
#include <linux/timex.h>
#include "clockddrvr.h"

MODULE_LICENSE("GPL");

static struct proc_dir_entry* j_file;

//  time stamp counter

static int j_show(struct seq_file *m, void *v)
{
	int i = 0;
	for(i=0; i<6; i++) {
		volatile unsigned long cycles_low_start, cycles_high_start;
		volatile unsigned cycles_low_end, cycles_high_end;

		volatile int start = jiffies;
		volatile int timeout = start + HZ/2; // let it run 1 second

		asm volatile (
			"CPUID\n\t"
			"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (cycles_high_start), "=r" (cycles_low_start)::
					"%rax", "%rbx", "%rcx", "%rdx");

		while(jiffies < timeout) {}
		volatile int finish = jiffies;

		asm volatile (
			"CPUID\n\t"
			"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (cycles_high_end), "=r" (cycles_low_end)::
					"%rax", "%rbx", "%rcx", "%rdx");
/*
		seq_printf(m, "Clock finish (@jiffies=%ld,HZ=%d) ticks=%ld %u %i seconds\n" ,
				finish,HZ, cycles_high_end, cycles_low_end, (finish-start)/HZ );

		seq_printf(m, "Clock start  (@jiffies=%ld,HZ=%d) ticks=%ld %u \n" ,
				start,HZ, cycles_high_start, cycles_low_start);

		seq_printf(m, "high time   end ticks= %u \n" , cycles_high_end * 100 );
		seq_printf(m, "low time    end ticks= %u \n" , cycles_low_end / 100000000);

		seq_printf(m, "high time start ticks= %u \n" , cycles_high_start * 100);
		seq_printf(m, "low time  start ticks= %u \n" , cycles_low_start / 100000000);
*/
		unsigned long completed =  (cycles_high_end * 100  ) +(cycles_low_end / 100000000);
		unsigned long initial = (cycles_high_start * 100 ) +  (cycles_low_start / 100000000);
		unsigned long totalticks =  completed  - initial;

//		seq_printf(m, " time       end ticks= %u \n" , completed);
//		seq_printf(m, " time     start ticks= %u \n" , initial);
//		seq_printf(m, "total ticks %u \n" , totalticks );
		seq_printf(m, "total ticks %u \n" , 2*totalticks);
	}
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
