#include "OS/mutex.h"

static uint32_t notificationCounter = 0;
OS_TCB_t* list_pop_tail_sl(_OS_tasklist_t *list);

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
	OS_TCB_t * task = list_pop_tail_sl(&mutex->waitingList);
	if (task) {
		list_push_sl(&pending_list, task);
	}
}

/*
	1. Atomically load the current head
	2. If list is empty, return null
	3. If the list contains only one item, atomically set the head to null.
		 Return the only item (head and tail of the list)
	4. Find tail->prev. If the list has more than one item, find node before tail.
		 Doing it this way because it's hard to find tail in a singly linked list.
	5. Making sure that we're updating the list atomically. do...while will only do this
		 if the exclusive flag is set.
	6. After do...while, remove the tail and set a new tail in the list.
	7. Pop and return the correct tail as needed.
*/
OS_TCB_t* list_pop_tail_sl(_OS_tasklist_t *list) {
	OS_TCB_t *oldHead = 0;
	OS_TCB_t *current = 0;
	do {
		// atomically load the current head of the list
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		// if the list is empty, return null
		if (!oldHead) {
			return 0;
		}
		// if there's only one item in the list
		if (!oldHead->next) {
			// attempt to atomically set the list head to null
			if (__STREXW((uint32_t)0, (uint32_t volatile *)&(list->head)) == 0) {
				return oldHead; // successfully removed the only item
			}
			continue; // exclusive access was lost, retry
		}
		// find the second-to-last item in the list
		current = oldHead;
		while (current->next && current->next->next) {
			current = current->next;
		}
		// the above loop exits with 'current' being the second-to-last node
	}	
	while (__STREXW((uint32_t)oldHead, (uint32_t volatile *)&(list->head)));
	// 'current' is now the second-to-last node, and 'tail' is the last node
	OS_TCB_t *tail = current->next;
	// remove the last node from the list
	current->next = 0;
	// return the removed tail node
	return tail;
}
