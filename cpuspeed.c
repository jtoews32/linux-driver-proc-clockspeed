
#include <linux/init.h>
#include <linux/module.h>	// for init_module()
#include <linux/proc_fs.h>	// for create_proc_read_entry()
#include <linux/pci.h>		// for pci_get_device()
#include <asm/uaccess.h>	// for put_user()
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

#define CLASS_SATA_NEW 0x010601	// for SATA Mass Storage
#define CLASS_SATA_OLD 0x01018F	// for SATA Mass Storage

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x27C1	// ICH7 SATA Controller
//#define DEVICE_ID	0x2922	// ICH7 SATA Controller

char modname[] = "cpuspeed";
struct pci_dev	*devp;
static struct proc_dir_entry* proc;


static ssize_t my_info(struct file *file, char *buf, size_t _len, loff_t *ppos)
{
	int	i, len = 0;

	len += sprintf( buf+len, "\n PCI Configuration Space -- " );
	len += sprintf( buf+len, "Intel Serial-ATA " );
	len += sprintf( buf+len, "Mass Storage Controller \n" );
	for (i = 0; i < 0x100; i += 4 )
		{
		u32	datum;
		pci_read_config_dword( devp, i, &datum );
		if ( (i % 32) == 0 )
			len += sprintf( buf+len, "\n 0x%02X: ", i );
		len += sprintf( buf+len, "%08X ", datum );
		}
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "\n" );

	return	len;
}

static const struct file_operations proc_fops =
{
 .owner = THIS_MODULE,
 .read  = my_info,

};

static int my_init (void)
{
	printk( "<1>\nInstalling \'%s\' module \n", modname );

	if ( !devp ) devp = pci_get_class( CLASS_SATA_NEW, NULL );
	if ( !devp ) devp = pci_get_class( CLASS_SATA_OLD, NULL );
	if ( !devp ) return -ENODEV;

	printk( " SATA Controller: vendor=%04X device=%04X \n",
		devp->vendor, devp->device );
	if ( devp->vendor != VENDOR_ID ) return -ENODEV;


	proc = proc_create( "cpuspeed",  0777, NULL, &proc_fops);

	if (!proc) {
		return -ENOMEM;
	}

	return	0;
}

static void my_exit (void)
{
	remove_proc_entry( "cpuspeed", NULL );

	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init (my_init);
module_exit (my_exit);
