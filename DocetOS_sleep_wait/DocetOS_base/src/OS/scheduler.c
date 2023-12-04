#define OS_INTERNAL

#include "OS/scheduler.h"
#include "OS/os.h"

#include "stm32f4xx.h"

#include <string.h>


/* This is an implementation of an extremely simple round-robin scheduler.

   A task list structure is declared.  Tasks are added to the list to create a circular buffer.
	 When tasks finish, they are removed from the list.  When the scheduler is invoked, it simply
	 advances the head pointer, and returns it.  If the head pointer is null, a pointer to the
	 idle task is returned instead.
	 
	 The scheduler is reasonably efficient but not very flexible.  The "yield" flag is not
	 checked, but merely cleared before a task is returned, so OS_yield() is equivalent to
	 OS_schedule() in this implementation.
*/

static _OS_tasklist_t task_list = {.head = 0};
static _OS_tasklist_t wait_list = {.head = 0};
static _OS_tasklist_t pending_list = {.head = 0};
// Need to make the sleep list an insertion sorted list
// The head of the list is basically the task that is ready to be woken up every time
static _OS_tasklist_t sleep_list = {.head = 0};
static uint32_t notificationCounter = 0;

uint32_t notification_counter(void) {
	return notificationCounter;
}

static void list_add(_OS_tasklist_t *list, OS_TCB_t *task) {
	if (!(list->head)) {
		task->next = task;
		task->prev = task;
		list->head = task;
	} else {
		task->next = list->head;
		task->prev = list->head->prev;
		task->prev->next = task;
		list->head->prev = task;
	}
}

static void list_remove(_OS_tasklist_t *list, OS_TCB_t *task) {
	if (task->next == task) {
		list->head = 0;
		return;
	}
	else if (list->head == task) {
		list->head = task->next;
	}
	task->prev->next = task->next;
	task->next->prev = task->prev;
}

// Push an item into a singly-linked list.
static void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task) {
	// Loop until the task is added to the list
	do {
		// atomically load the current head of list and mark the memory address for exclusive access
		// also return the value at that address
		OS_TCB_t *head = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		// set the "next" pointer of new task to current head of the list
		// the task will be inserted at the beginning of the list
		task->next = head;
	}
	// attempts to store the new task as the new head of the list atomically
  // __STREXW will complete the operation only if the exclusive access mark is still valid.
  // If exclusive access is still valid (returns 0).
  // If access is lost (returns 1), goes back to the start of the "do" statement.
	while (__STREXW ((uint32_t) task, (uint32_t volatile *)&(list->head)));
}

static OS_TCB_t* list_pop_sl(_OS_tasklist_t *list) {
	// track the head
	OS_TCB_t * oldHead = NULL;
	do {
		// atomically load the current head and set it as oldHead
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		if (!oldHead) {
			// clear the exclusive access flag, so the STREXW works correctly later on
			__CLREX();
			return 0;
		}
	}
	// attempt to update the head of list atomically.
	while (__STREXW ((uint32_t) oldHead->next, (uint32_t volatile *)&(list->head)));
	// clear the "next" pointer of the old head.
	// this prevents the popped task from pointing to elements in the list at all. Avoids dangling pointers.
	oldHead->next = NULL;
	return oldHead;
}

void OS_notifyAll(void) {
	notificationCounter++;
	OS_TCB_t * task = 0;
	// removes all tasks from wait list and adds them to the pending list
	/* assigning whatever is popped from the wait list to "task", as well as checking if it
		 is the same as the item that was popped.
		 Needed because inbetween each loop, the wait list could get altered by other interrupt */
	while ( (task = list_pop_sl(&wait_list)) ) {
		list_push_sl(&pending_list, task);
	}
}

static void sort_sleep_list(OS_TCB_t *insertTask) {
	// extract the wake-up time from the task's data field.
	uint32_t taskWakeUpTime = *(uint32_t *)(insertTask->data);
  // check if the list is empty or if this task needs to wake up earlier 
  // than the current head of the list.	
	if(!sleep_list.head || taskWakeUpTime <= *(uint32_t *)(sleep_list.head->data)) {
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
	while (currentTask->next && *(uint32_t *)(currentTask->next->data) <= taskWakeUpTime) {
		currentTask = currentTask->next;
	}
	// Insert the task in its correct position in the list.
	// The task is inserted after 'current' and before 'current->next'.
	insertTask->next = currentTask->next;		// The new task points to the next task in the list.
	currentTask->next = insertTask;					// The current task now points to the new task.
}

/* Round-robin scheduler */
OS_TCB_t const * _OS_schedule(void) {
	// removes all tasks from pending list and adds them to the round-robin list
	while (pending_list.head) {
		list_add(&task_list, list_pop_sl(&pending_list));
	}

	// wake up tasks if needed
	while (sleep_list.head && *(uint32_t *)(sleep_list.head->data) <= OS_elapsedTicks()) {
		OS_TCB_t *taskToWake = list_pop_sl(&sleep_list);
		taskToWake->state &= ~TASK_STATE_SLEEP;
		list_add(&task_list, taskToWake);
	}
	
	// Check if there is at least one task in the round-robin list.
	if (task_list.head) {
		// Store the original head to detect if we've gone full circle through the list.
		OS_TCB_t *originalHead = task_list.head;
		do {
			// Advance to the next task in the list.
			task_list.head = task_list.head->next;

			// If the task is runnable or if its wake-up time has passed, it should be scheduled.
			if (!(task_list.head->state & TASK_STATE_SLEEP)) {
				// Clear the yield flag since we're about to schedule this task now.
				task_list.head->state &= ~TASK_STATE_YIELD;
				// Return the current task, which is either not asleep or has just been woken up.
				return task_list.head;
			}
			// Continue looping until we've checked all tasks in the list.
		} while (task_list.head != originalHead);
	}
	// If no runnable tasks are found, or all tasks are asleep, return the idle task.
  return _OS_idleTCB_p;
}

/* Initialises a task control block (TCB) and its associated stack.  See os.h for details. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data) {
	TCB->sp = stack - (sizeof(_OS_StackFrame_t) / sizeof(uint32_t));
	TCB->state = 0;
	TCB->prev = TCB->next = 0;
	_OS_StackFrame_t *sf = (_OS_StackFrame_t *)(TCB->sp);
	/* By placing the address of the task function in pc, and the address of _OS_task_end() in lr, the task
	   function will be executed on the first context switch, and if it ever exits, _OS_task_end() will be
	   called automatically */
	*sf = (_OS_StackFrame_t) {
		.r0 = (uint32_t)(data),
		.r1 = 0,
		.r2 = 0,
		.r3 = 0,
		.r4 = 0,
		.r5 = 0,
		.r6 = 0,
		.r7 = 0,
		.r8 = 0,
		.r9 = 0,
		.r10 = 0,
		.r11 = 0,
		.r12 = 0,
		.lr = (uint32_t)_OS_task_end,
		.pc = (uint32_t)(func),
		.psr = xPSR_T_Msk  /* Sets the thumb bit to avoid a big steaming fault */
	};
}

/* 'Add task' */
void OS_addTask(OS_TCB_t * const tcb) {
	list_add(&task_list, tcb);
}

/* SVC handler that's called by _OS_task_end when a task finishes.  Removes the
   task from the scheduler and then queues PendSV to reschedule. */
void _OS_taskExit_delegate(void) {
	// Remove the given TCB from the list of tasks so it won't be run again
	OS_TCB_t * tcb = OS_currentTCB();
	list_remove(&task_list, tcb);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void _OS_wait_delegate(void * const stack) {
	_OS_SVC_StackFrame_t *svcStack = (_OS_SVC_StackFrame_t *)stack;
	if (svcStack->r0 == notificationCounter) {
		OS_TCB_t * currentTask = task_list.head;
		list_remove(&task_list, currentTask);
		list_push_sl(&wait_list, currentTask);
		
		// setting the PendSV bit to trigger a context switch
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

void _OS_sleep_delegate(uint32_t sleepTime) {
	// current task's TCB
	OS_TCB_t *currentTask = OS_currentTCB();
	// calculate when the task would need to wake up 
	uint32_t wakeUp = OS_elapsedTicks() + sleepTime;
	// store the wake up time in the TCB
	currentTask->data = &wakeUp;
	// indicate the current task state to be sleeping
	currentTask->state |= TASK_STATE_SLEEP;
	
	list_remove(&task_list, currentTask);
	sort_sleep_list(currentTask);
	
	// setting the PendSV bit to trigger a context switch
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

