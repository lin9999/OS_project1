#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_my_printk(const char *msg) {
	return printk(msg);
} 