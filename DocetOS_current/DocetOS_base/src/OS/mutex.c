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
		else if (head != currentTCB) {
			// Call OS_wait() since the mutex is currently held by another task.
			OS_wait(notificationCounter, (uint32_t)&(mutex->waitingList));
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
			OS_notify(mutex);
		}
	}
	// Yield the processor after releasing the mutex to allow other tasks to run.
	// This is important for ensuring fair scheduling and preventing a single task
	// from hogging the CPU.
	OS_yield();
}

void _OS_wait_delegate(_OS_SVC_StackFrame_t *svcStack) {
	_OS_tasklist_t *waitingList = (_OS_tasklist_t *)(svcStack->r1);

	if (svcStack->r0 == notificationCounter) {
		OS_TCB_t * currentTask = OS_currentTCB();
		list_remove(&task_list, currentTask);
		list_push_sl(waitingList, currentTask);
    
		// setting the PendSV bit to trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

void OS_notify(OS_mutex_t *mutex) {
	notificationCounter++;
	OS_TCB_t * task = list_pop_sl(&mutex->waitingList);
	if (task) {
		list_push_sl(&pending_list, task);
	}
}

/*
	We need to be pushing onto the head of the list and popping the tail of the list instead.
	At the moment we are pushing and popping the head of the waiting list, hence why the tasks don't 
	run as expected without the "OS_sleep". 
	Also, the waiting list needs to be a doubly linked list instead of a single linked, since
	we need to know where the tail of the list is.
	Because when popping from the list, we can just do "list.head->prev" or something similar.
	This should hopefully fix the task mutex issue without the "OS_sleep" call in main.c
*/

