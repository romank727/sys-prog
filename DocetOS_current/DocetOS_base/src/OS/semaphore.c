#include "OS/semaphore.h"

OS_TCB_t* list_pop_tail_sl_smph(_OS_tasklist_t *list);

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

void OS_semaphore_release(OS_semaphore_t * semaphore) {
	if (__LDREXW(&(semaphore->token))) {
		if (!(__STREXW(0, &(semaphore->token)))) {
			// Check if there are tasks waiting on this semaphore
			OS_TCB_t * task = list_pop_tail_sl_smph(&semaphore->waitingList);
			if (task) {
				list_push_sl(&pending_list, task);
			}
		}
	}
	OS_yield();
}

void _OS_wait_semph_delegate(_OS_SVC_StackFrame_t *svcStack) {
	_OS_tasklist_t *waitingList = (_OS_tasklist_t *)(svcStack->r1);
	
	if (svcStack->r0) {
		OS_TCB_t *currentTask = OS_currentTCB();
		// Remove the current task from its priority queue
		if (currentTask->priority < MAX_PRIORITY_LEVELS) {
			list_remove(&task_queues[currentTask->priority], currentTask);
		}
		// Semaphore is not available, add the current task to the semaphore's waiting list
		list_push_sl(waitingList, currentTask);
		// Trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

OS_TCB_t* list_pop_tail_sl_smph(_OS_tasklist_t *list) {
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
