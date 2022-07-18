
/* add by qingcheng*/
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/timer.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,14)
#include <asm/system.h>
#endif
#include <asm/uaccess.h>
#include <asm/mach-ralink/surfboardint.h>
#include <linux/interrupt.h>
#include "rt_timer.h"

#include <asm/mach-ralink/rt_mmap.h>
#define RALINK_SYSCTL_ADDR     RALINK_SYSCTL_BASE  // system control
#define RALINK_REG_GPIOMODE    (RALINK_SYSCTL_ADDR + 0x60)
#define RALINK_PRGIO_ADDR      RALINK_PIO_BASE // Programmable I/O
#define RALINK_REG_PIODIR    (RALINK_PRGIO_ADDR + 0x00)
#define RALINK_REG_PIODATA    (RALINK_PRGIO_ADDR + 0x20)
#define RALINK_REG_PIOSET     (RALINK_PRGIO_ADDR + 0x30)
#define RALINK_REG_PIORESET   (RALINK_PRGIO_ADDR + 0x40)
#define RALINK_REG(x)  (*((volatile u32 *)(x)))

#define PAT_WDT_GPIO   29            // for h721
#define PAT_WDT_500MS	(HZ/2)

/* first stage: manual feed */
/* third stage: watchdog program feed */
static int pat_wdt_state = 0;
void pat_wdt_feed( void )
{
    if ( pat_wdt_state != 0 )
    {
        if ( PAT_WDT_GPIO < 32 )
        {
            RALINK_REG(RALINK_REG_PIODIR) |= (1<<PAT_WDT_GPIO);
            RALINK_REG(RALINK_REG_PIODATA) &= ~(1<<PAT_WDT_GPIO);
        }
        else
        {
            unsigned int t = 0;
            t = PAT_WDT_GPIO - 32;
            RALINK_REG(RALINK_REG_PIODIR+4) |= (1<<t);
            RALINK_REG(RALINK_REG_PIODATA+4) &= ~(1<<t);
        }
        pat_wdt_state = 0;
    }
    else
    {
        if ( PAT_WDT_GPIO < 32 )
        {
            RALINK_REG(RALINK_REG_PIODIR) |= (1<<PAT_WDT_GPIO);
            RALINK_REG(RALINK_REG_PIODATA) |= (1<<PAT_WDT_GPIO);
        }
        else
        {
            unsigned int t = 0;
            t = PAT_WDT_GPIO - 32;
            RALINK_REG(RALINK_REG_PIODIR+4) |= (1<<t);
            RALINK_REG(RALINK_REG_PIODATA+4) |= (1<<t);
        }
        pat_wdt_state = 1;
    }
}
EXPORT_SYMBOL_GPL( pat_wdt_feed );

/* second stage: early feed */
//static int wdg_early_count = 600; //10 min
static int wdg_early_count = 300; //5 min
static struct timer_list wdg_timer;

void pat_wdt_early_disable( void )
{
	del_timer( &wdg_timer );
}
EXPORT_SYMBOL_GPL( pat_wdt_early_disable );

static void pat_wdt_handle( unsigned long unused )
{
	if ( wdg_early_count-- > 0 )
    {
		pat_wdt_feed();
    	wdg_timer.expires = jiffies + PAT_WDT_500MS;
    	add_timer( &wdg_timer );
	}
	else
    {
		pat_wdt_early_disable();
    }
}
void pat_wdt_early_init( void )
{
	init_timer( &wdg_timer );
	wdg_timer.data = 0;
	wdg_timer.function = pat_wdt_handle;
	wdg_timer.expires = jiffies + PAT_WDT_500MS;
    add_timer( &wdg_timer );
}

