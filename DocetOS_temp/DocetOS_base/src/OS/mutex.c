#include "OS/mutex.h"

static uint32_t notificationCounter = 0;

uint32_t notification_counter(void) {
	return notificationCounter;
}

void OS_mutex_acquire (OS_mutex_t * mutex) {
	OS_TCB_t *currentTCB = OS_currentTCB();
	// Enter an infinite loop which will be exited manually when the mutex is acquired.
	while(1) {
		// Use LDREXW to atomically load the mutex's TCB field.
		OS_TCB_t *head = (OS_TCB_t *)__LDREXW((uint32_t volatile *)&(mutex->taskBlock));
		// If the mutex's TCB field is zero, try to acquire the mutex.
		if (!head) {
			// Use STREXW to try to store the current TCB in the mutex's TCB field.
			if (!(__STREXW((uint32_t)currentTCB, (uint32_t volatile *)&(mutex->taskBlock)))) {
				break;	// If STREX succeeds, break out of the loop - the mutex is acquired.
			}
			// If STREXW fails, the loop will continue, trying to acquire the mutex again.
		}
		// If the mutex's TCB field is not zero and not equal to the current TCB:
		else if (head != currentTCB) {
			list_push_sl((_OS_tasklist_t *)(&(mutex->waitingList)), currentTCB);
			// Call OS_wait() since the mutex is currently held by another task.
			OS_wait(notificationCounter);
		}
	}
	// Increment the mutex's counter after acquiring it.
	mutex->counter++;
}

void OS_mutex_release (OS_mutex_t * mutex) {
	// Check if the current task is the one that holds the mutex.
	if (mutex->taskBlock == OS_currentTCB()) {
		// Decrement the mutex counter, indicating a release attempt.
		mutex->counter--;
		// If the counter reaches zero, it means there are no more nested locks on the mutex.
		if (!mutex->counter) {
			// Set the mutex's task block to zero, effectively freeing the mutex.
			mutex->taskBlock = 0;
			if (mutex->waitingList.head) {
				notificationCounter++;
				OS_TCB_t * nextTask = list_pop_sl((_OS_tasklist_t *)(&(mutex->waitingList)));
				mutex->taskBlock = nextTask;
				list_push_sl(&pending_list, mutex->taskBlock);
			}
		}
	}
	// Yield the processor after releasing the mutex to allow other tasks to run.
	// This is important for ensuring fair scheduling and preventing a single task
	// from hogging the CPU.
	OS_yield();
}

void _OS_wait_delegate(_OS_SVC_StackFrame_t *svcStack) {
	if (svcStack->r0 == notificationCounter) {
		OS_TCB_t * currentTask = task_list.head;
		list_remove(&task_list, currentTask);
		//list_push_sl(&wait_list, currentTask);
		
		// setting the PendSV bit to trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}


/* 
	 1. have a list of TCBs that are ready to get the mutex. 
	 2. once a TCB frees up the mutex, the head of the list will contain 
			the task that is ready to acquire the mutex (be notified)
	 3. no other TCBs will be notified, they simply move up the order of the list. 
	 4. have some sort of counter like we did before to show the release attempt maybe.
*/
