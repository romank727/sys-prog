#define OS_INTERNAL

#include "OS/sleep.h"
#include "OS/os.h"

// Function to perform correct insertion sort into "sleeping list".
void sort_sleep_list(void *taskToInsert) {
	OS_TCB_t * insertTask = (OS_TCB_t *) taskToInsert;
	// extract the wake-up time from the task's data field.
	uint32_t taskWakeUpTime = insertTask->data;
  // check if the list is empty or if this task needs to wake up earlier 
  // than the current head of the list.	
	if (!sleep_list.head || taskWakeUpTime <= sleep_list.head->data) {
		// If the list is empty or the task wakes up earlier than the current first task,
		// use list_push_sl to add it to the front of the list.
		list_push_sl(&sleep_list, insertTask);
		return;
	}
	// iterate from the head of the list.
	OS_TCB_t *currentTask = sleep_list.head;
	// Iterate through the list until finding the position where the task should be inserted.
	// This is done by comparing the wake-up times of the tasks in the list.
	// The loop continues until it finds a task that wakes up later than the current task.
	while (currentTask->next && (currentTask->next->data <= taskWakeUpTime)) {
		currentTask = currentTask->next;
	}
	// Insert the task in its correct position in the list.
	// The task is inserted after 'current' and before 'current->next'.
	insertTask->next = currentTask->next;		// The new task points to the next task in the list.
	currentTask->next = insertTask;					// The current task now points to the new task.
}

void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack) {
	// current task's TCB
	OS_TCB_t *currentTask = OS_currentTCB();
	// calculate when the task would need to wake up 
	uint32_t wakeUp = OS_elapsedTicks() + svcStack->r0;
	// store the wake up time in the TCB
	currentTask->data = wakeUp;
	// indicate the current task state to be sleeping
	currentTask->state |= TASK_STATE_SLEEP;
	
	// Check if the task's priority is within the valid range
	if (currentTask->priority < MAX_TASK_PRIORITY_LEVELS) {
		// Remove the current task from its priority queue
		list_remove(&task_queues[currentTask->priority], currentTask);
	}
	
	// put the task into the sleep list while also keeping it sorted on insertion
	sort_sleep_list(currentTask);
	
	// setting the PendSV bit to trigger a context switch
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
