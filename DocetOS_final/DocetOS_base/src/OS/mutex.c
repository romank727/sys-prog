#define OS_INTERNAL

#include "OS/mutex.h"
#include "stm32f4xx.h"

/**
* A global counter used as a check code for mutex operations. This counter is incremented
* every time a task is notified (via OS_notify) that a mutex is available. It helps in
* implementing fail-fast behavior in the mutex waiting mechanism. When a task attempts to
* wait for a mutex, it first checks this counter. If the counter has changed by the time
* the task is about to enter the waiting state, it indicates that a notification has occurred
* in the meantime, allowing the task to quickly exit the wait operation and avoid potential
* deadlock situations.
*/
static uint32_t notificationCounter = 0;

/**
*	Attempts to atomically acquire a mutex. If the mutex is already held,
*	the calling task will wait until it becomes available. Supports re-entrant
*	behaviour, which allows the same task to acquire the mutex multiple times.
*
*	@param	mutex		Pointer to the 'OS_mutex_t' struct, which represents
*									the mutex to be acquired.
*	
*	1.	Retrieve the current task's TCB using 'OS_currentTCB()'.
*	2.	Enter an infinite loop to attempt acquiring the mutex:
*				-	Atomically load the mutex's TCB (head) using LDREX. This checks
*					if the mutex is currently held.
*				-	If the mutex is not held (head is NULL), attempt to atomically set 
*					the mutex's TCB to the current task using STREX.
*				-	If the atomic operation succeeds (mutex is acquired), exit the loop.
*				-	If the mutex is already held by another task, call 'OS_wait' to wait 
*					for the mutex. This is done via an SVC call to a delegate.
*				-	If the mutex is held by the current task (re-entrant), exit the loop
*					immediately, allowing nested mutex locks. 
*	3.	Increment the mutex's counter after acquiring it, marking the mutex as held.
*/
void OS_mutex_acquire (OS_mutex_t * mutex) {
	OS_TCB_t *currentTCB = OS_currentTCB();
	while(1) {
		OS_TCB_t *head = (OS_TCB_t *)__LDREXW((uint32_t volatile *)&(mutex->taskBlock));
		if (!head) {
			// Use STREXW to try to store the current TCB in the mutex's TCB field.
			if (!(__STREXW((uint32_t)currentTCB, (uint32_t volatile *)&(mutex->taskBlock)))) {
				break;	// Mutex successfully acquired, exit the loop
			}
		}
		// If the mutex is already held by the current task (re-entrant)
		else if (head == currentTCB) {
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

/**
*	Releases a mutex that was previously acquired by a task. If this release operation
*	results in the mutex becoming available, it notifies any tasks waiting for the mutex. 
*	After releasing the mutex, it yields the processor to allow other tasks to be scheduled.
*
*	@param	mutex		Pointer to the mutex being released.
*
*	1.	Decrement the mutex's counter. This counter tracks the number of times
*			the mutex has been acquired re-entrantly by the same task. A non zero value
*			means the mutex is still held due to nested locking. 
*	2.	Check if the counter reaches zero, which shows that there are no more nested locks
*			and the mutex is fully released.
*	3.	If the mutex is fully released:
*				-	Set the mutex's TCB pointer to zero, meaning that the mutex is no longer
*					held by any task
*				-	Call 'OS_notify' to wake up one of the tasks waiting on this mutex, if any. This will
*					move the task from the waiting list to the pending list, making it ready for scheduling.
*	4.	Yield the processor by calling 'OS_yield'. This ensures overall fairness, making sure that
*			other tasks get to run. Without yielding, the current task might continue execting and
*			acquiring the mutex before othre tasks get a chance.
*			In other words, prevents a single task from hogging the CPU.
*/
void OS_mutex_release (OS_mutex_t * mutex) {
	mutex->counter--;
	if (!mutex->counter) {
		mutex->taskBlock = 0;
		OS_notify(mutex);
	}
	OS_yield();
}

/**
*	SVC delegate function to handle waiting for a mutex. This is invoked
*	as a result of 'OS_wait_mutex'. It checks if the mutex is available. If it's not,
*	it places the task in the mutex's waiting list.
*
*	@param	svcStack		Pointer to the stack frame structure, containing
*											SVC call parameters and context.
*
*	1.	Extract the mutex's waiting list address from the stack frame.
*	2.	Check if the current task's check code matches the 'notificationCounter'.
*				i)	If they match, it means no notify operation has occurred since the check code
*						was obtained, so the task needs to wait further.
*				ii) If they don't match, a notify operation has occurred, so the task shouldn't wait.
*	3.	If the task needs to wait:
*				-		Obtain task's current TCB
*				-		Remove the task from its current priority queue, preventing it from being scheduled
*				-		Add the task to the mutex's waiting list. 
*				-		Set the PendSV bit, which triggers an interrupt. This results in a context switch, allowing
*						the processor to switch to another task since the current one is now waiting.
*/
void _OS_wait_mutex_delegate(_OS_SVC_StackFrame_t *svcStack) {
	OS_tasklist_t *waitingList = (OS_tasklist_t *)(svcStack->r1);

	if (svcStack->r0 == notificationCounter) {
		OS_TCB_t * currentTask = OS_currentTCB();
		if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
			// Remove the current task from its priority queue
			list_remove(&task_queues[currentTask->priority], currentTask);
		}
		list_push_sl(waitingList, currentTask);
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

/**
*	Notifies a task that is waiting on a mutex. This function is called when 
*	a mutex is released. It removes the task from the mutex's waiting list and adds it
*	to the pending list, making it ready for further scheduling.
*	
*	@param	mutex		Pointer to mutex on which tasks might be waiting.
*
*	1.	Increment the 'notificationCounter'. This counter acts as a check-code for mutex
*			operations, allowing tasks to detect changes to the mutex, which could alter
*			the waiting behaviour. 
*	2.	Pop a task from the tail of the mutex's waiting list. The 'list_pop_tail_sl' function
*			is used to atomically remove the last task in the waiting list. This choice
*			ensures that tasks are unblocked in the order they were blocked, preserving fairness.
*	3.	If a task is successfully popped from the waiting list (list isn't empty), it is
*			then pushed onto the pending list using 'list_push_sl'. This allows the task to be 
*			further scheduled.
*
*	Note:	The use of 'list_pop_tail_sl' and 'list_push_sl' makes sure atomicity is preserved.
*/
void OS_notify(OS_mutex_t *mutex) {
	notificationCounter++;
	OS_TCB_t * task = list_pop_tail_sl(&mutex->waitingList);
	if (task) {
		list_push_sl(&pending_list, task);
	}
}
