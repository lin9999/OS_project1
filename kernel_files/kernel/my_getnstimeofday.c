#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/timer.h>

asmlinkage void sys_my_getnstimeofday(struct timespec *ts) {
	getnstimeofday(ts);
}