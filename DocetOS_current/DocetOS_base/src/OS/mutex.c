#define OS_INTERNAL

#include "OS/mutex.h"

static uint32_t notificationCounter = 0;

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
		else if (head == currentTCB) {
			// address the issue if the same task tries to get the mutex
			break;
		}
		else {
			// Call OS_wait() since the mutex is currently held by another task.
			OS_wait_mutex(notificationCounter, (uint32_t)&(mutex->waitingList));
		}
	}
	// Increment the mutex's counter after acquiring it.
	mutex->counter++;
}

void OS_mutex_release (OS_mutex_t * mutex) {
	mutex->counter--;
	// If the counter reaches zero, it means there are no more nested locks on the mutex.
	if (!mutex->counter) {
		// Set the mutex's task block to zero, effectively freeing the mutex.
		mutex->taskBlock = 0;
		OS_notify(mutex);
	}
	// Yield the processor after releasing the mutex to allow other tasks to run.
	// This is important for ensuring fair scheduling and preventing a single task
	// from hogging the CPU.
	OS_yield();
}

void _OS_wait_mutex_delegate(_OS_SVC_StackFrame_t *svcStack) {
	OS_tasklist_t *waitingList = (OS_tasklist_t *)(svcStack->r1);

	if (svcStack->r0 == notificationCounter) {
		OS_TCB_t * currentTask = OS_currentTCB();
		// Check if the task's priority is within the valid range
		if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
			// Remove the current task from its priority queue
			list_remove(&task_queues[currentTask->priority], currentTask);
		}
		list_push_sl(waitingList, currentTask);
		// setting the PendSV bit to trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

void OS_notify(OS_mutex_t *mutex) {
	notificationCounter++;
	OS_TCB_t * task = list_pop_tail_sl(&mutex->waitingList);
	if (task) {
		list_push_sl(&pending_list, task);
	}
}
