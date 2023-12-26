#define OS_INTERNAL

#include "OS/semaphore.h"

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
			OS_TCB_t * task = list_pop_tail_sl(&semaphore->waitingList);
			if (task) {
				list_push_sl(&pending_list, task);
			}
		}
	}
	OS_yield();
}

void _OS_wait_semph_delegate(_OS_SVC_StackFrame_t *svcStack) {
	OS_tasklist_t *waitingList = (OS_tasklist_t *)(svcStack->r1);
	
	if (svcStack->r0) {
		OS_TCB_t *currentTask = OS_currentTCB();
		// Remove the current task from its priority queue
		if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
			list_remove(&task_queues[currentTask->priority], currentTask);
		}
		// Semaphore is not available, add the current task to the semaphore's waiting list
		list_push_sl(waitingList, currentTask);
		// Trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}
