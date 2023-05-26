#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/cred.h>
// Define the array_stats struct for the array_stats syscall.
#ifndef _ARRAY_STATS_H_
#define _ARRAY_STATS_H_

asmlinkage long sys_array_stats(struct array_stats *stats, long *data, long size);
struct array_stats {
  long min;
  long max;
  long sum;
};

#endif
