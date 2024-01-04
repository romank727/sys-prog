#define OS_INTERNAL

#include "OS/sleep.h"
#include "OS/os.h"
#include "stm32f4xx.h"

/**
* Retrieves the total number of system ticks elapsed since the system started.
*/ 
extern uint32_t OS_elapsedTicks(void);

/**
*	Performs an insertion sort on the sleep list to place a task in the 
*	correct position based on its wake-up time. This ensures that tasks in the
*	sleep list are sorted in ascending order of their wake-up times, which allows
*	for a more efficient process once the tasks need to wake up. 
*
*	@param	taskToInsert	The task to be inserted into the sleep list. This
*												should be a pointer to an 'OS_TCB_t' structure.
*
*	1.	Extract the wake-up time from the task's data field.
*	2.	Check if the sleep list is empty of if the task needs to wake up
*			earlier than the current head of the list. If it does, insert the task
*			at the beginning of the list.
*	3.	If the list isn't empty and the task doesn't wake up earlier than the head,
*			iterate through the list to find the correct insertion point. This is done by
*			comparing the wake up times of the tasks in the list.
*	4.	Continue traversing the list until a task is found that wakes up later than the
*			task to be inserted.
*	5.	Insert the task at this position:
*				-	The task is placed after the current task and before the next task in the list.
*					This maintains the sorted order in the list.
*/
void i_sort_sleep_list(void *taskToInsert) {
	OS_TCB_t * insertTask = (OS_TCB_t *) taskToInsert;
	uint32_t taskWakeUpTime = insertTask->data;
  // check if the list is empty or if this task needs to wake up earlier 
  // than the current head of the list.	
	if (!sleep_list.head || taskWakeUpTime <= sleep_list.head->data) {
		list_push_sl(&sleep_list, insertTask);
		return;
	}
	// iterate from the head of the list.
	OS_TCB_t *currentTask = sleep_list.head;
	while (currentTask->next && (currentTask->next->data <= taskWakeUpTime)) {
		currentTask = currentTask->next;
	}
	// The task is inserted after 'current' and before 'current->next'.
	insertTask->next = currentTask->next;		// The new task points to the next task in the list.
	currentTask->next = insertTask;					// The current task now points to the new task.
}

/**
*	SVC delegate function for handling task sleep requests. This function finds the
*	wake-up time for the current task based on the requested sleep duration and then places
*	the task into the sleep list using insertion sort.
*
*	@param	svcStack	Pointer to the SVC stack frame, which contains
*										the sleep duration passed by the stack.
*
*	1.	Obtain the current task's TCB.
*	2.	Calculate the wake-up time by adding the sleep duration (svcStack->r0) to
*			the current elapsed ticks found from 'OS_elapsedTicks'.
*	3.	Store the calculated wake-up time in the task's data field.
*	4.	Set the task state to indicate it's now sleeping.
*	5.	If the task's priority is within the valid range, remove the task
*			from its current queue in the scheduler.
*	6.	Insert the task into the sleep list using 'i_sort_sleep_list', which performs
*			an insertion to place the task in the right position in the list. 
*	7.	Set the PendSV bit to trigger a context switch, which will allow the 
*			scheduler to run a new task.
*/
void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack) {
	OS_TCB_t *currentTask = OS_currentTCB();
	uint32_t wakeUp = OS_elapsedTicks() + svcStack->r0;
	currentTask->data = wakeUp;
	currentTask->state |= TASK_STATE_SLEEP;
	
	if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
		list_remove(&task_queues[currentTask->priority], currentTask);
	}
	
	i_sort_sleep_list(currentTask);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
