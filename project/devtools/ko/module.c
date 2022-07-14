/*
 *    Description:  Linux内核模块
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

/* 内核模块初始化函数 */
int __init module_register( void )
{
    printk("fpktools kernel module register\n");

    /* ############################################################### */
    /* ############### 此处可增加自已的代码  ####################### */
    /* ############################################################### */

    /* 示例调用相同目录下function.c文件的函数 */
    kernel_module_func( KERNEL_MODULE_MACRO_1, 20 );

    return 0;
}
/* 内核模块注销函数 */
void __exit module_unregister( void )
{
    printk("fpktools kernel module unregister\n");

    /* ############################################################### */
    /* ############## 此处可增加自已的代码 ########################### */
    /* ############################################################### */

    /* 示例调用相同目录下function.c文件的函数 */
    kernel_module_func( KERNEL_MODULE_MACRO_1, 40 );

}

/* 内核模块初始化函数注册 */
module_init(module_register);
/* 内核模块注销函数注册 */
module_exit(module_unregister);

/* 内核模块信息 */
MODULE_AUTHOR("fpktools");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("fpktools kernel module generate by fpktools");

