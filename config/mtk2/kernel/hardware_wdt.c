#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/sgi/mc.h>

#define WATCHDOG_TIMEOUT 2		/* 2 sec default timeout */
#define WATCHDOG9_MINOR	 129

#ifdef CONFIG_WATCHDOG_NOWAYOUT
static int nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");
#endif

/* add by dimmalex for hwwdt feed */
extern void pat_wdt_feed( void );
extern void pat_wdt_early_disable( void );


/*
 *	Allow only one person to hold it open
 */
static int hardware_wdt_open(struct inode *inode, struct file *file)
{
    /* add by dimmalex for hwwdt */
	//printk("Turn off early pat watchdog refush\n");
	pat_wdt_early_disable();

	return nonseekable_open(inode, file);
}

static int hardware_wdt_release(struct inode *inode, struct file *file)
{
	/* Shut off the timer.
	 * Lock it in if it's a module and we defined ...NOWAYOUT */
	pat_wdt_feed();
	return 0;
}

static ssize_t hardware_wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	/* Refresh the timer. */
	if (len) {
        pat_wdt_feed();
	}
	return len;
}

static struct watchdog_info ident = {
    .options		= WDIOF_KEEPALIVEPING,
    .identity		= "Ralink Hardware WatchDog",
};

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
long hardware_wdt_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int hardware_wdt_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
#endif
{
	int options, retval = -EINVAL;

	switch (cmd) {
		default:
			return -ENOTTY;
		case WDIOC_GETSUPPORT:
			if (copy_to_user((struct watchdog_info *)arg, &ident, sizeof(ident)))
				return -EFAULT;
			return 0;
		case WDIOC_GETSTATUS:
		case WDIOC_GETBOOTSTATUS:
			return put_user(0,(int *)arg);
		case WDIOC_KEEPALIVE:
            pat_wdt_feed();

			return 0;
		case WDIOC_GETTIMEOUT:
			return put_user(WATCHDOG_TIMEOUT,(int *)arg);
		case WDIOC_SETOPTIONS:
		{
			if (get_user(options, (int *)arg))
				return -EFAULT;

			if (options & WDIOS_DISABLECARD) {
				retval = 0;
			}

			if (options & WDIOS_ENABLECARD) {
				retval = 0;
			}

			return retval;
		}
	}
}

static int hardware_wdt_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{
    pat_wdt_feed();

	return NOTIFY_DONE;
}

static const struct file_operations hardware_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= hardware_wdt_write,
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
	.unlocked_ioctl = hardware_wdt_ioctl,
#else
	.ioctl		= hardware_wdt_ioctl,
#endif
	.open		= hardware_wdt_open,
	.release	= hardware_wdt_release,
};

static struct miscdevice hardware_wdt_miscdev = {
	.minor		= WATCHDOG9_MINOR,
	.name		= "watchdog9",
	.fops		= &hardware_wdt_fops,
};

static struct notifier_block hardware_wdt_notifier = {
	.notifier_call = hardware_wdt_notify_sys,
};

static char banner[] __initdata =
	KERN_INFO "Ralink APSoC Hardware Watchdog Timer\n";

static int __init hardware_wdt_init(void)
{
	int ret;

	ret = register_reboot_notifier(&hardware_wdt_notifier);
	if (ret) {
		printk(KERN_ERR "cannot register reboot notifier (err=%d)\n",
			ret);
		return ret;
	}

	ret = misc_register(&hardware_wdt_miscdev);
	if (ret) {
		printk(KERN_ERR "cannot register miscdev on minor=%d (err=%d)\n",
			WATCHDOG9_MINOR, ret);
		unregister_reboot_notifier(&hardware_wdt_notifier);
		return ret;
	}

	printk(banner);

	return 0;
}

static void __exit hardware_wdt_exit(void)
{
	misc_deregister(&hardware_wdt_miscdev);
	unregister_reboot_notifier(&hardware_wdt_notifier);
}

module_init(hardware_wdt_init);
module_exit(hardware_wdt_exit);

MODULE_AUTHOR("Steven Liu,qingcheng");
MODULE_DESCRIPTION("Ralink APSoC Hardware Watchdog Timer");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG9_MINOR);
