#include "array_stats.h"
#include <linux/kernel.h>
#include <linux/syscall.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>



// Implement an array_stats system call
sys_array_stats(struct array_stats *stats, long* data, long size) {

    long *curValPtr;
    long curVal = *curValPtr;

    long maxVal = curVal;
    long minVal = curVal;
    long sumVals = curVal;

    struct array_stats *localStats;

    // return -EINVAL if size <= 0
    if(size <= 0) {
        return -EINVAL;
    };

    // copy stats array to a local version, error if failed to copy
    if(copy_from_user(localStats, stats, sizeof(stats)) != 0) {
        return -EFAULT;
    };

    // initialize variables to first element in data
    if(copy_from_user(curValPtr, data, sizeof(long)) != 0) {
        return -EFAULT;
    };

    for(int i = 1; i < size; i++) {
        // set curVal = current value in the data array
        if(copy_from_user(curValPtr, data + i, sizeof(long)) != 0) {
            return -EFAULT;
        };
        curVal = *curValPtr;

        // update maxVal/minVal/sumVals
        if(curVal > maxVal) {
            maxVal = curVal;
        }
        if(curVal < minVal) {
            minVal = curVal;
        }
        sumVals += curVal;
    };

    localStats->min = minVal;
    localStats->max = maxVal;
    localStats->sum = sumVals;
    
    if(copy_to_user(stats, localStats, sizeof(stats)) != 0) {
        return -EFAULT;
    };

    // success case
    return 0;
};
