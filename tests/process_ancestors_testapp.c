// Test application for the array_stats syscall.

//#include "array_stats.h"
//#include "process_ancestors.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

// Sys-call number:
#define _ARRAY_STATS_ 437
#define _PROCESS_ANCESTORS_ 438
#define length 20

// Structure to hold values returned by process_ancestors sys-call


#define ANCESTOR_NAME_LEN 16
struct process_info {
 long pid;                     /* Process ID */
 char name[ANCESTOR_NAME_LEN]; /* Program name of process */
 long state;                   /* Current process state */
 long uid;                     /* User ID of process owner */
 long nvcsw;                   /* # of voluntary context switches */
 long nivcsw;                  /* # of involuntary context switches */
 long num_children;            /* # of children processes */
 long num_siblings;            /* # of sibling processes */
};

// Define the array_stats struct for the array_stats syscall.


struct array_stats {
  long min;
  long max;
  long sum;
};


/**
 * Prototypes
 */

void test_positive_few(void);
void test_negative_few(void);
void test_many(void);
void test_bad_addr(void);
static void do_syscall_working(long data[], long size);
static void do_syscall_failing(struct array_stats *stats, long data[],
                               long size, long ret_code);
static void test_internal(_Bool success, int lineNum, char *argStr);
static void test_print_summary(void); 

//tests for process_anchestors

void test_null_pointer(void) {
    long num_filled;
    struct process_info info_array[length];
    int result = syscall(_PROCESS_ANCESTORS_, NULL, length, &num_filled);
    assert(result == -1);
}
void test_zero_size(void) {
    long num_filled;
    struct process_info info_array[length];
    int result = syscall(_PROCESS_ANCESTORS_, info_array, 0, &num_filled);
    assert(result == -1);
}

void test_null_num_filled(void) {
    struct process_info info_array[length];
    int ret = syscall(_PROCESS_ANCESTORS_, info_array, length, NULL);
    assert(ret == -1);
}

void test_valid_input(void) {
    const long num_filled;
    struct process_info info_array[length];
    int result = syscall(_PROCESS_ANCESTORS_, info_array, length, &num_filled);
    printf("result = %d\n", result);
    assert(result == 0);
    assert(num_filled <= length && num_filled>=0);
    for (int i = 0; i < num_filled; i++) {
        assert(info_array[i].pid > 0);
        assert(strlen(info_array[i].name) > 0);
        assert(info_array[i].state >= 0);
        assert(info_array[i].uid >= 0);
        assert(info_array[i].nvcsw >= 0);
        assert(info_array[i].nivcsw >= 0);
        assert(info_array[i].num_children >= 0);
        assert(info_array[i].num_siblings >= 0);
    }
}



/***********************************************************
 * main()
 ***********************************************************/
int main(int argc, char *argv[]) {
   test_positive_few();
  test_negative_few();
  test_many();

  test_bad_addr();

  test_print_summary(); 

  //tests for process anchestors
  test_null_pointer();
  test_zero_size();
  test_null_num_filled();
  test_valid_input();
  printf("All tests for process ancestors passed \n");


  return 0;
}

/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
 void test_positive_few() {
  do_syscall_working((long[]){1}, 1);
  do_syscall_working((long[]){1, 2}, 2);
  do_syscall_working((long[]){1, 2, 3}, 3);
  do_syscall_working((long[]){0, 2, 4, 6}, 4);
  do_syscall_working((long[]){5, 3, 4, 1}, 4);
}
void test_negative_few() {
  do_syscall_working((long[]){-1}, 1);
  do_syscall_working((long[]){-1, -2}, 2);
  do_syscall_working((long[]){-1, 2, 3}, 3);
  do_syscall_working((long[]){0, -2, 4, -6}, 4);
}

void fill(long data[], long size) {
  for (int i = 0; i < size; i++) {
    data[i] = rand();
    if (i % 2 == 0) {
      data[i] *= -1;
    }
  }
}
#define MEG (1024 * 1024)
void test_many() {
  for (int i = 1; i <= 5; i++) {
    long size = MEG * i;
    long *data = malloc(sizeof(data[0]) * size);
    fill(data, size);
    do_syscall_working(data, size);
    free(data);
  }
}

void test_bad_addr() {
  struct array_stats stats;
  // 0 or negative sizes
  do_syscall_failing(&stats, (long[]){1}, 0, EINVAL);
  do_syscall_failing(&stats, (long[]){1}, -1, EINVAL);
  do_syscall_failing(&stats, (long[]){1}, -10000, EINVAL);

  // Bad data pointers
  do_syscall_failing(&stats, NULL, 1, EFAULT);
  do_syscall_failing(&stats, (long *)1LL, 1, EFAULT);
  do_syscall_failing(&stats, (long *)123456789012345689LL, 1, EFAULT);

  // Bad size (read off end)
  do_syscall_failing(&stats, (long[]){1}, 10 * MEG, EFAULT);

  // Bad stats pointers, or read-only memory
  do_syscall_failing(NULL, (long[]){1}, 1, EFAULT);
  do_syscall_failing((void *)1, (long[]){1}, 1, EFAULT);
  do_syscall_failing((void *)test_bad_addr, (long[]){1}, 1, EFAULT);
}
 
/***********************************************************
 * Custom testing framework
 ***********************************************************/
// Track results:
static int numTests = 0;
static int numTestPassed = 0;

static int current_syscall_test_num = 0;
static int last_syscall_test_num_failed = -1;
static int num_syscall_tests_failed = 0;

// Macro to allow us to get the line number, and argument's text:
#define TEST(arg) test_internal((arg), __LINE__, #arg)

// Actual function used to check success/failure:
static void test_internal(_Bool success, int lineNum, char *argStr) {
  numTests++;
  if (!success) {
    if (current_syscall_test_num != last_syscall_test_num_failed) {
      last_syscall_test_num_failed = current_syscall_test_num;
      num_syscall_tests_failed++;
    }
    printf("-------> ERROR %4d: test on line %d failed: %s\n", numTestPassed,
           lineNum, argStr);
  } else {
    numTestPassed++;
  }
}

static void test_print_summary(void) {
  printf("\nExecution finished.\n");
  printf("%4d/%d tests passed.\n", numTestPassed, numTests);
  printf("%4d/%d tests FAILED.\n", numTests - numTestPassed, numTests);
  printf("%4d/%d unique sys-call testing configurations FAILED.\n",
         num_syscall_tests_failed, current_syscall_test_num);
} 

/***********************************************************
 * Routines to double check array answers
 ***********************************************************/
 static long find_max(long data[], long size) {
  long max = data[0];
  for (int i = 0; i < size; i++) {
    if (data[i] > max) {
      max = data[i];
    }
  }
  return max;
}
static long find_min(long data[], long size) {
  long min = data[0];
  for (int i = 0; i < size; i++) {
    if (data[i] < min) {
      min = data[i];
    }
  }
  return min;
}
static long find_sum(long data[], long size) {
  long sum = 0;
  for (int i = 0; i < size; i++) {
    sum += data[i];
  }
  return sum;
} 

/***********************************************************
 * Functions to actually make the sys-call and test results
 ***********************************************************/
 static int do_syscall(struct array_stats *stats, long *data, long size) {
  current_syscall_test_num++;
  printf("\nTest %d: ..Diving to kernel level\n", current_syscall_test_num);
  int result = syscall(_ARRAY_STATS_, stats, data, size);
  int my_errno = errno;
  printf("..Rising to user level w/ result = %d", result);
  if (result < 0) {
    printf(", errno = %d", my_errno);
  } else {
    my_errno = 0;
  }
  printf("\n");
  return my_errno;
}
static void do_syscall_working(long data[], long size) {
  struct array_stats stats;
  int result = do_syscall(&stats, data, size);
  printf("Stats: min = %ld, max = %ld, sum = %ld\n", stats.min, stats.max,
         stats.sum);

  TEST(result == 0);
  TEST(stats.min == find_min(data, size));
  TEST(stats.max == find_max(data, size));
  TEST(stats.sum == find_sum(data, size));
}
static void do_syscall_failing(struct array_stats *stats, long data[],
                               long size, long ret_code) {
  int result = do_syscall(stats, data, size);
  TEST(result == ret_code);
} 
