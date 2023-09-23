/*
 * Ralink MT7621/MT7628 built-in hardware watchdog timer
 *
 * Copyright (C) 2014 John Crispin <john@phrozen.org>
 *
 * This driver was based on: drivers/watchdog/rt2880_wdt.c
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>

extern int watchdog_dev_register(struct watchdog_device *wdd); 
extern int watchdog_dev_unregister(struct watchdog_device *wdd); 

/* add by qingcheng */
//extern void hard_wdt_early_disable(void); 
//extern void hard_wdt_feed(void);

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		 "Watchdog cannot be stopped once started (default="
		 __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

#define WDG_TIMER_DFT_CNT	30	//feed watchdog hand 15 second
extern void hard_wdt_set_count(int count);

static int hardware_wdt_ping(struct watchdog_device *w)
{
	//hard_wdt_early_disable();
	//hard_wdt_feed();

	hard_wdt_set_count(WDG_TIMER_DFT_CNT);
	return 0;
}

static int hardware_wdt_set_timeout(struct watchdog_device *w, unsigned int t)
{
	hardware_wdt_ping(w);
	return 0;
}

static int hardware_wdt_start(struct watchdog_device *w)
{
	return 0;
}

static int hardware_wdt_stop(struct watchdog_device *w)
{
	hardware_wdt_set_timeout(w, w->timeout);
	return 0;
}

static const struct watchdog_info hardware_wdt_info = {
	.identity = "Hardware Watchdog",
	.options = WDIOF_KEEPALIVEPING,
};

static const struct watchdog_ops hardware_wdt_ops = {
	.owner = THIS_MODULE,
	.start = hardware_wdt_start,
	.stop = hardware_wdt_stop,
	.ping = hardware_wdt_ping,
	.set_timeout = hardware_wdt_set_timeout,
};

static struct watchdog_device hardware_wdt_dev = {
	.info = &hardware_wdt_info,
	.ops = &hardware_wdt_ops,
	.min_timeout = 1,
	.max_timeout = 0xfffful / 1000,
	.id = 9,
};

#if 1
static int hardware_wdt_probe(struct platform_device *pdev)
{
	int ret;

	//printk("MediaTek hardware watchdog probe\n");
	watchdog_init_timeout(&hardware_wdt_dev, hardware_wdt_dev.max_timeout,
			      &pdev->dev);
	watchdog_set_nowayout(&hardware_wdt_dev, nowayout);

	ret = watchdog_dev_register(&hardware_wdt_dev);

	//ret = watchdog_register_device(&hardware_wdt_dev);
	printk("MediaTek hardware watchdog init Done %d\n", ret);

	return 0;
}

static int hardware_wdt_remove(struct platform_device *pdev)
{
	watchdog_dev_unregister(&hardware_wdt_dev);

	return 0;
}

static void hardware_wdt_shutdown(struct platform_device *pdev)
{
	//hardware_wdt_stop(&hardware_wdt_dev);
}
#if 0
static const struct of_device_id hardware_wdt_match[] = {
	{ .compatible = "mediatek,hardware-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, hardware_wdt_match);
#endif

#define HARDDOG_NAME "hardware_wdog"

static struct platform_driver hardware_wdt_driver = {
	.probe		= hardware_wdt_probe,
	.remove		= hardware_wdt_remove,
	.shutdown	= hardware_wdt_shutdown,
	.driver		= {
		.name		= HARDDOG_NAME,
	},
};
#endif
//module_platform_driver(hardware_wdt_driver);

static int __init hardware_wdt_init(void)
{
	struct platform_device *pdev;
	int ret;

	printk("MediaTek hardware watchdog init\n");
	pdev = kzalloc(sizeof(*pdev), GFP_KERNEL);
	if (!pdev) {
		return -ENOMEM;
	}

	pdev->name = HARDDOG_NAME;
	pdev->id = 0;
	ret = platform_device_register(pdev);
//	printk("MediaTek hardware watchdog register device %d\n", ret);
	if (ret) {
		kfree(pdev);
		return -ENODEV;
	}

	ret = platform_driver_register(&hardware_wdt_driver);
	return ret;
}

static void __exit hardware_wdt_exit(void)
{
	watchdog_unregister_device(&hardware_wdt_dev);
	return;
}




module_init(hardware_wdt_init);
module_exit(hardware_wdt_exit);

MODULE_DESCRIPTION("MediaTek hardware watchdog driver");
MODULE_AUTHOR("qingcheng <qingchengrd@outlook.org");
MODULE_LICENSE("GPL v2");
