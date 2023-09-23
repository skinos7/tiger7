
/* add by qingcheng*/
#include <linux/io.h>
#include <asm/addrspace.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/interrupt.h>

#ifdef CONFIG_SOC_RT288X
#define EARLY_PIO_BASE		0xA0300600
#elif defined(CONFIG_SOC_MT7621)
#define EARLY_PIO_BASE		0x1E000600
#define EARLY_SYSCTL_BASE	0x1E000000
#else
#define EARLY_PIO_BASE		0x10000600
#endif

#define HARD_WDT_GPIO   	25            // for v520
//#define GPIO_MODE_VAL		15		
#define HARD_WDT_500MS	(HZ/2)

static __iomem void *pio_membase = (__iomem void *) KSEG1ADDR(EARLY_PIO_BASE);
//static __iomem void *sysc_membase = (__iomem void *) KSEG1ADDR(EARLY_SYSCTL_BASE);

static inline void gpio_w32(u32 val, unsigned reg)
{
	__raw_writel(val, pio_membase + reg);
}

static inline u32 gpio_r32(unsigned reg) 
{
	return __raw_readl(pio_membase + reg);
}
#if 0
static inline void sysc_w32(u32 val, unsigned reg)
{
	__raw_writel(val, sysc_membase + reg);
}

static inline u32 sysc_r32(unsigned reg)
{
	return __raw_readl(sysc_membase + reg);
}
#endif
#define TIME_TO_COUNT(x)	2*(x)

static int gpio_num = -1;
static int gpio_dir_reg = 0x0;
static int gpio_set_reg = 0x30;
static int gpio_reset_reg = 0x40;
static int wdt_mode = 0; //0: intv feed  1: fast feed
static int wdt_early_count = TIME_TO_COUNT(600); //600seconds
static struct timer_list wdt_timer;

static void get_wdt_gpio(void)
{
	gpio_num = HARD_WDT_GPIO;
	return;
}
#if 0
void hard_wdt_init(void)
{
	unsigned int value;

	value = sysc_r32(0x60);
	value = value | (1 << GPIO_MODE_VAL);
	sysc_w32(value, 0x60);
		
}
#endif

void hard_wdt_feed_intv(void)
{
	unsigned int val;
	static int wdt_flags = 0;

	val = gpio_r32(gpio_dir_reg);
	gpio_w32(val | 1 << gpio_num, gpio_dir_reg);

	if (wdt_flags) {
		gpio_w32(1 << gpio_num, gpio_set_reg);
	} else {
		gpio_w32(1 << gpio_num, gpio_reset_reg);
	}
	wdt_flags = ~wdt_flags;
}

void hard_wdt_feed_fast(void)
{
	unsigned int val;

	val = gpio_r32(gpio_dir_reg);
	gpio_w32(val | 1 << gpio_num, gpio_dir_reg);
	gpio_w32(1 << gpio_num, gpio_set_reg);
	ndelay(100);
	gpio_w32(1 << gpio_num, gpio_reset_reg);
}

void hard_wdt_feed(void)
{
	if (gpio_num < 0) {
		get_wdt_gpio();
		/* mt7621 gpio is 0 to 96 */
		gpio_dir_reg = (gpio_num/32)*4;
		gpio_set_reg = (gpio_num/32)*4 + 0x30;
		gpio_reset_reg = (gpio_num/32)*4 + 0x40;
		/* get gpio num in banks*/
		gpio_num = HARD_WDT_GPIO%32;
		//hard_wdt_init();
	} else {
		if (wdt_mode) {
			hard_wdt_feed_fast();
		} else {
			hard_wdt_feed_intv();
		}
	}
}
EXPORT_SYMBOL_GPL(hard_wdt_feed);

void hard_wdt_set_count(int count)
{
	wdt_early_count = count;
}
EXPORT_SYMBOL_GPL(hard_wdt_set_count);


static void hard_wdt_handle(struct timer_list *unused)
{
	if (wdt_early_count-- > 0){
		hard_wdt_feed();
    	wdt_timer.expires = jiffies + HARD_WDT_500MS;
    	add_timer(&wdt_timer);
	} 
}

//static DEFINE_TIMER(wdt_timer, hard_wdt_handle);
void hard_wdt_early_init( void )
{
	timer_setup(&wdt_timer, hard_wdt_handle, 0);
	wdt_timer.expires = jiffies + HARD_WDT_500MS;
    add_timer( &wdt_timer );
}

void hard_wdt_oops_on(int val)
{
	del_timer_sync(&wdt_timer);
}
EXPORT_SYMBOL_GPL(hard_wdt_oops_on);
