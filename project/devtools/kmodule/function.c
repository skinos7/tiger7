/*
 *    Description:  示例Linux内核模块函数
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

/* 常用的内核模块头文件 */
#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/device.h>         
#include "function.h"
/* ############################################################### */
/* ################ 此处可增加更多的内核头文件 ################### */
/* ############################################################### */



/* ############################################################### */
/* ############## 此处可增加自已的代码 ########################### */
/* ############################################################### */

/* 示例函数 */
void  kernel_module_func( int c, int i )
{
    printk("kernel_module_func( %d, %d ) called\n", c, i );
}

/* 内核模块信息 */
MODULE_LICENSE("GPL");

