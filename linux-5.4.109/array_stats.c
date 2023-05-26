#include "array_stats.h"
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/cred.h>
#include <linux/syscalls.h>
// Implement an array_stats system call
SYSCALL_DEFINE3(array_stats, struct array_stats *, stats, long *, data, long , size) {
    if(data == NULL) {
		return -EFAULT;
	}
    //printk("First number is %ld\n", data[0]);
   // long *curValPtr;
    long curVal;


    struct array_stats localStats;
    // return -EINVAL if size <= 0
    if(size <= 0) {
        return -EINVAL;
    };
    if(stats == NULL) {
		return -EFAULT;
	}
    // copy stats array to a local version, error if failed to copy
    // if(copy_from_user(localStats, stats, sizeof(stats)) != 0) {
    //     return -EFAULT;
    // };

    // initialize variables to first element in data
    if(copy_from_user(&curVal, &data[0], sizeof(long))) {
        return -EFAULT;
    };
    long maxVal; 
    maxVal = curVal;
    long minVal;
    minVal= curVal;
    long sumVals; 
    sumVals = curVal;
   // printk("TEST 5\n");
    int i;
    for(i = 1; i < size; i++) {
        // set curVal = current value in the data array
        //printk("TEST 6\n");
        if(copy_from_user(&curVal, &data[i], sizeof(long))) {
            return -EFAULT;
        };
        //printk("TEST 7\n");

        // update maxVal/minVal/sumVals
        if(curVal > maxVal) {
            maxVal = curVal;
        }
        if(curVal < minVal) {
            minVal = curVal;
        }
        sumVals += curVal;
    };
    //printk("TEST 8\n");
    localStats.min = minVal;
    localStats.max = maxVal;
    localStats.sum = sumVals;
    //printk("min number is %ld\n", localStats.min);


    if(copy_to_user(&stats->sum, &localStats.sum, sizeof(long))) {
        return -EFAULT;
    };
    if(copy_to_user(&stats->min, &localStats.min, sizeof(long))) {
        return -EFAULT;
    };
    if(copy_to_user(&stats->max, &localStats.max, sizeof(long))) {
        return -EFAULT;
    };
    

    // success case
    return 0;
};
