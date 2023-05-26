# System-calls-in-Linux

## The Array Statistics Syscall

First, added a new system call that computes some basic statistics (max, min, sum) on an array of
data. In practice, it makes little sense to have this as a syscall; but it allows to become
familiar with accessing memory between user and kernel space before solving more complex and real
problems that involve the interaction between kernel and user space.

### Specifications

In the kernel's `cmpt300` directory, created a file named `array_stats.h`.

Then create a new syscall named `array_stats` (function `sys_array_stats()`):

* Implemented it in kernel's `cmpt300` directory in a file named `array_stats.c`, using
  previously defined `array_stats.h` header file: `#include "array_stats.h"`.
* Assigned syscall number 437 to the new syscall (in `syscall_64.tbl`).
* The syscall's signature is:

  ```
  asmlinkage long sys_array_stats(struct array_stats *stats, long *data, long size);
  ```

  * `stats`: A pointer to one `array_stats` structure allocated by the user-space application.
      Structure will be written to by the syscall to store the minimum, maximum, and sum of all
      values in the array pointed to by data.  
  * `data`: An array of `long int` values passed in by the user-space application.  
  * `size`: The number of elements in data. Must be greater than 0.

## The Process Ancestors Syscall

In this section, implemented a syscall which returns information about the current process,
plus its ancestors (its parent process, it's grandparent process, and so on).

Then created a new syscall named `process_ancestors` (function `sys_process_ancestors()`):

* Implemented it in kernel's `cmpt300` directory in a file named `process_ancestors.c`, using previously defined header file: `#include "process_ancestors.h"`.
* Assigned syscall number 438 to the new syscall (in `syscall_64.tbl`).
* The syscall's signature is:

  ```
  asmlinkage long sys_process_ancestors(struct process_info *info_array,
                                        long size,
                                        long *num_filled)
  ```

  * `info_array`: An array of `process_info` structs that will be written to by the kernel as it
    fills in information from the current process on up through its ancestors.  
  * `size`: : The number of structs in `info_array`. This is the maximum number of structs that the
    kernel will write into the array (starting with the current process as the first entry and
    working up from there). The size may be larger or smaller than the actual number of ancestors of
    the current process: larger means some entries are left unused (see `num_filled`); smaller means
    information about some processes not written into the array.  
  * `num_filled`: A pointer to a `long` integer. To this location the kernel will store the number
    of structs (in `info_array`) which are written by the kernel. May be less than size if the
    number of ancestors for the current process is less than size.

The `process_ancestors` syscall is:

* Starting at the current process, fill the elements in `info_array` with the correct values.
* Ordering: the current process's information goes into `info_array[0]`; the parent of the current
  process into `info_array[1]`; grandparent into `info_array[2]` and so on.
* Extra structs in `info_array` are left unmodified.
* Return 0 when successful.
* Returns `-EINVAL` if size <= 0.
* Returns `-EFAULT` if any problems access `info_array` or `num_filled`.
* It must not allocate or use a large amount of kernel memory to copy/store large arrays into.

Finally, created a user-space test program which calls your syscall and exercises its functionality. 
