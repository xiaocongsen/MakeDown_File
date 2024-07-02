# 内核模块

## 代码

hello_module.c

```c
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_ALERT */

int init_module(void)
{
    printk("<1>Hello world 1.\n" );
/*
* A non 0 return means init_module failed; module can't be loaded.
*/
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_ALERT "Goodbye world 1.\n" );
}

```

```c
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_ALERT */

int my_init_module(void)
{
        printk("<1>Hello world 111111.\n" );
/*
* A non 0 return means init_module failed; module can't be loaded.
*/
        return 0;
}

void my_cleanup_module(void)
{
        printk(KERN_ALERT "Goodbye world 1111111.\n" );
}
module_init(my_init_module);
module_exit(my_cleanup_module);
```

```c
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_ALERT */
#include <linux/init.h> /* Needed for the macros */
static int hello3_data __initdata = 3;          //宏__initdata 同__init 类似，只不过对变量有效。
static int __init hello_3_init(void)
{
printk(KERN_ALERT "Hello, world %d\n", hello3_data);
return 0;
}
static void __exit hello_3_exit(void)
{
printk(KERN_ALERT "Goodbye, world 3\n" );
}
module_init(hello_3_init);
module_exit(hello_3_exit);
```

## Makefile

```Makefile
# Makefile for compiling the kernel module

# Name of the kernel module (without .ko extension)
MODULE_NAME := hello_module

# Specify the path to the kernel source tree
KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build

# Set the flags for the kernel build
obj-m := $(MODULE_NAME).o

# Specify the rules for building the module
all:
    make -C $(KERNEL_SOURCE) M=$(PWD) modules

# Specify the rules for cleaning up the build
clean:
    make -C $(KERNEL_SOURCE) M=$(PWD) clean

```

## 编译

    make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
    编译成功后得到hello_module.ko

## 使用

    sudo insmod hello_module.ko     加载内核模块
    dmesg                           查看内核日志
    sudo rmmod hello_module         卸载内核模块
