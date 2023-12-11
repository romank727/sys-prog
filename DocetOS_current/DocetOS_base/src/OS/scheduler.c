#define OS_INTERNAL

#include "OS/os.h"
#include "OS/sleep.h"
#include "stm32f4xx.h"

/* This is an implementation of an extremely simple round-robin scheduler.

   A task list structure is declared.  Tasks are added to the list to create a circular buffer.
	 When tasks finish, they are removed from the list.  When the scheduler is invoked, it simply
	 advances the head pointer, and returns it.  If the head pointer is null, a pointer to the
	 idle task is returned instead.
	 
	 The scheduler is reasonably efficient but not very flexible.  The "yield" flag is not
	 checked, but merely cleared before a task is returned, so OS_yield() is equivalent to
	 OS_schedule() in this implementation.
*/

_OS_tasklist_t task_list = {.head = 0};
_OS_tasklist_t pending_list = {.head = 0};
_OS_tasklist_t sleep_list = {.head = 0};

void list_add(_OS_tasklist_t *list, OS_TCB_t *task) {
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

void list_remove(_OS_tasklist_t *list, OS_TCB_t *task) {
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
void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task) {
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

OS_TCB_t* list_pop_sl(_OS_tasklist_t *list) {
	// track the head
	OS_TCB_t * oldHead = 0;
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
	return oldHead;
}

/* Round-robin scheduler */
OS_TCB_t const * _OS_schedule(void) {
	// wake up tasks if needed
	while (sleep_list.head && (sleep_list.head->data <= OS_elapsedTicks())) {
		OS_TCB_t *taskToWake = list_pop_sl(&sleep_list);
		taskToWake->state &= ~TASK_STATE_SLEEP;
		list_push_sl(&pending_list, taskToWake);
	}
	
	// removes all tasks from pending list and adds them to the round-robin list
	while (pending_list.head) {
		list_add(&task_list, list_pop_sl(&pending_list));
	}
	
	// Check if there is at least one task in the round-robin list.
	if (task_list.head) {
		// Advance to the next task in the list.
		task_list.head = task_list.head->next;
		// Clear the yield flag since we're about to schedule this task now.
		task_list.head->state &= ~TASK_STATE_YIELD;
		// Return the current task, which is either not asleep or has just been woken up.
		return task_list.head;
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
