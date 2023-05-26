#include "process_ancestors.h"
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/cred.h>
#include <linux/syscalls.h>



SYSCALL_DEFINE3(process_ancestors, struct process_info __user *, info_array, long, size, long __user *, num_filled) {									
	//declaring current number of structs filled
	long curr_filled = 0;
	long children = 0;
	long sibling = 0;
	struct task_struct* curr_task = current;
	struct list_head *c_head = &curr_task->children;
	struct list_head *c_next = c_head->next;
	struct list_head *s_head = &curr_task->sibling;
	struct list_head *s_next = s_head->next;

    if (size <= 0) {
		return -EINVAL;
	}
	if(info_array == NULL){
		return -EFAULT;
	}

	

	//declaring current process struct
	
	
	while(curr_task->parent != curr_task || curr_filled == 0){
		//counting number of childrens
		c_head = &curr_task->children;
		c_next = c_head->next;
		while (c_next != c_head)
		{
			children++;
			c_next = c_next->next;
		}

		
		//counting number of siblings
		s_head = &curr_task->sibling;
		s_next = s_head->next;
		while (s_next != s_head)
		{
			sibling++;
			s_next = s_next->next;
		}
		printk("pid: %d, name: %s, state: %li, uid: %d, nvcsw: %li, nivscw: %li, num_children: %li, num_siblings: %li\n",
			curr_task->pid, curr_task->comm, curr_task->state, (curr_task->cred)->uid.val, curr_task->nvcsw, curr_task->nivcsw, children, sibling);

		//store everything into info_array
		if(curr_filled < size){
			struct process_info temp;
			temp.pid = curr_task->pid;
			strcpy(temp.name, curr_task->comm);
			temp.state = curr_task->state;
			temp.uid = (curr_task->cred)->uid.val;
			temp.nvcsw = curr_task->nvcsw;
			temp.nivcsw = curr_task->nivcsw;
			temp.num_children = children;
			temp.num_siblings = sibling;

			if(copy_to_user(&info_array[curr_filled], &temp, sizeof(struct process_info))){
				return -EFAULT;
			}
			curr_filled++;
		}
		children = 0;
		sibling = 0;


		curr_task = curr_task->parent;
				

	}
	if(copy_to_user(num_filled, &curr_filled, sizeof(long)))
		return -EFAULT;

    
	return 0;

}
