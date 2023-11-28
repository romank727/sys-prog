#include "OS/mutex.h"
#include <stdint.h>

void OS_mutex_acquire (OS_mutex_t * mutex) {
	OS_TCB_t * currentTCB = OS_currentTCB();
	do {
		OS_TCB_t * head = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(mutex->taskBlock));
		if (!head) {
			// continue forces program to go to while condition
			continue;
		}
		else if (head != currentTCB) {
			OS_wait();
			// clear the flag so STREXW fails, and we go back to step 1.
			__CLREX();
			continue;
		}
	}
	while (__STREXW ((uint32_t)currentTCB, (uint32_t volatile *)&(mutex->taskBlock)));
	mutex->counter++;
}

void OS_mutex_release (OS_mutex_t * mutex) {
	if (mutex->taskBlock == OS_currentTCB()) {
		mutex->counter--;
		if (!mutex->counter) {
			mutex->taskBlock = 0;
			OS_notifyAll();
		}
	}
	OS_yield();
}
