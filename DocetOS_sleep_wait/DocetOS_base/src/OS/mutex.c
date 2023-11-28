#include "OS/mutex.h"

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
			OS_wait();
			// Clear the exclusive lock set by LDREXW since we're going to wait.
			__CLREX();
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
			// Notify all tasks waiting for this mutex that it is now available.
			OS_notifyAll();
		}
	}
	// Yield the processor after releasing the mutex to allow other tasks to run.
	// This is important for ensuring fair scheduling and preventing a single task
	// from hogging the CPU.
	OS_yield();
}
