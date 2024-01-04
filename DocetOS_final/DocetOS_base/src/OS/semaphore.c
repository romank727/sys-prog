#define OS_INTERNAL

#include "OS/semaphore.h"
#include "stm32f4xx.h"

/**
*	Attempts to atomically acquire a semaphore for a task. If the semaphore is
*	already taken, the task will wait until the semaphore becomes available.
*
*	@param	semaphore		Pointer to the semaphore being acquired.
*
*	1.	Enter an infinite loop, which will exit once the semaphore is acquired.
*	2.	Atomically load the current state of the semaphore's token using LDREX, as well
*			as setting the exclusive flag.
*	3.	Check if the semaphore's token is free (token = 0).
*	4.	If semaphore is available:
*				-	Attempt to atomically set the token to 1 using STREX.
*				- If STREX operation succeeds, break out of the loop, which means the 
*					semaphore is acquired.
*				-	If STREX fails, it means exclusive flag is invalid, so the loop continues
*					until success.
*	5.	If semaphore is not available:
*				-	Call 'OS_wait_semaphore', passing the semaphore's token and its waiting list address.
*					This puts the current task into a waiting state until semaphore is released. 
*/
void OS_semaphore_acquire(OS_semaphore_t * semaphore) {
	while (1) {
		if (!(__LDREXW(&(semaphore->token)))) {
			if (!(__STREXW(1, &(semaphore->token)))) {
				break;
			}
		}
		else {
			OS_wait_semaphore(semaphore->token, (uint32_t)&(semaphore->waitingList));
		}
	}
}

/**
*	Releases a semaphore that is currently held by a task. This function also
*	checks if there are any tasks waiting for this semaphores and moves them
*	to the pending list for execution. 
*
*	@param	semaphore		Pointer to the semaphore being released.
*
*	1.	Atomically load semaphore's token using LDREX. This also sets an
*			exclusive flag.
*	2.	Check if the semaphore's token indicates that it is currently acquired.
*	3.	If the semaphore is currently acquired:
*				-	Attempt to atomically set semaphore's token to 0 using STREX.
*				-	If STREX succeeds, proceed to check for waiting tasks.
*				-	If STREX fails, this indicates that exclusive access was lost, so the
*					semaphore remains unchanged.
*	4.	If there are tasks waiting on this semaphore (checked after STREX):
*				-	Check and pop a task from the tail of the semaphore's waiting list.
*				-	Push the task onto the pending list.
*					This essentially re-schedules the task.
*	5.	Yield the processor. This allows the scheduler to move onto other tasks, ensuring
*			fairness and preventing a task from hogging the CPU.
*/
void OS_semaphore_release(OS_semaphore_t * semaphore) {
	if (__LDREXW(&(semaphore->token))) {
		if (!(__STREXW(0, &(semaphore->token)))) {
			// Check if there are tasks waiting on this semaphore
			OS_TCB_t * task = 0;
			if ( (task = list_pop_tail_sl(&semaphore->waitingList)) ) {
					list_push_sl(&pending_list, task);
			}
		}
	}
	OS_yield();
}

/**
*	Delegate function for 'OS_wait_semaphore' SVC call. It manages the
*	task waiting mechanism when a semaphore isn't available. If the semaphore is
*	currently acquired by another task, this function adds the current task to the
*	semaphore's waiting list and triggers a context switch.
*
*	@param	svcStack	Pointer to the stack frame passed by the SVC handler.
*
*	1.	Retrieve the waiting list for the semaphore from the stack frame.
*	2.	Check if the semaphore token (svcStack->r0) indicates that the semaphore
*			is not available.
*	3.	If the semaphore is not available:
*				-	Obtain the current task's TCB
*				-	Remove the current task from its priority queue if possible
*				-	Add the current task to the semaphore's waiting list, basically
*					putting it in a waiting state until semaphore becomes available.
*				-	Set the PendSV bit to trigger a context switch, allowing the scheduler
*					to execute a different task.
*/
void _OS_wait_semph_delegate(_OS_SVC_StackFrame_t *svcStack) {
	OS_tasklist_t *waitingList = (OS_tasklist_t *)(svcStack->r1);
	if (svcStack->r0) {
		OS_TCB_t *currentTask = OS_currentTCB();
		
		// Remove the current task from its priority queue
		if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
			list_remove(&task_queues[currentTask->priority], currentTask);
		}
		
		list_push_sl(waitingList, currentTask);
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}
