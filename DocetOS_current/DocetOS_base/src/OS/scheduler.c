#define OS_INTERNAL

#include "OS/os.h"
#include "stm32f4xx.h"

/*	Array of priority level task queues. 
		These queues hold TCBs that are ready to be executed,
		organised by their assigned priority.
		The highest priority level corresponds to a larger integer.
		e.g. '5' is of higher priority than '1'. */
OS_tasklist_t task_queues[MAX_TASK_PRIORITY_LEVELS];
/*	Temporary storage list for TCBs that are ready
		to be moved to their respective priority queues.
		Primarily used to hold TCBs that are transitioning
		from sleeping/waiting to ready-to-run. */
OS_tasklist_t pending_list = {.head = 0};
/*	Holds TCBs currently asleep. 
		Sleeping occurs for a specified amount of ticks,
		defined by the user.
		TCBs are relocated to 'pending_list' after sleeping. */
OS_tasklist_t sleep_list = {.head = 0};

/**
*	Adds a TCB to a specified doubly linked list.
*	The list is circular doubly linked, where each node
*	has a 'next' and 'prev' pointer.
*
* @param	list	Pointer to a list to which the task is added.
*								This should be a pointer to an initialised 'OS_tasklist_t' type.
*	@param	task	Pointer to TCB which will be added to the list.
*								This should be initialised and not be NULL.
*
*	1.	If the list is currently empty, the inserted TCB becomes the sole
*			element in the list. It's also set to point to itself, forming a circular
*			list with a single node. Setting the 'next' and 'prev' pointers as appropriate.
*	2.	If the list is already populated, the new task is inserted at the beginning
*			of the list. 'next' pointer is set to current head of the list and 'prev'
*			is set to last element of the list. The new task also becomes the head of the list.
*/
void list_add(OS_tasklist_t *list, OS_TCB_t *task) {
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

/**
*	Removes a TCB from a doubly linked list.
*	This function extracts a specified TCB from a given list.
*	The list is expected to be circular doubly linked.
*
*	@param	list	Pointer to the list from which the task is removed.
*								The list must be valid and initialised.
*	@param	task	Pointer to the TCB to be removed from the list.
*								The task must be part of the list and not NULL.
*
*	1.	If the task is the only element in the list, it sets the list's
*			head pointer to NULL, effectively emptying the list.
*	2.	If the task is the head of the list, updates the head pointer to
*			the next task in the list.
*	3.	The function adjusts the 'next' and 'prev' pointers of the adjacent tasks
*			in order to remove the TCB from the list. 
*
*	Note:	The function does not free the memory associated to the TCB, it simply
*				removes it from the list.
*/
void list_remove(OS_tasklist_t *list, OS_TCB_t *task) {
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

/**
*	Atomically pushes a TCB to the head of a singly linked list.
*	It tries to do so in a thread safe manner, using atomic instructions
*	to ensure the list remains consistent in a multi-threaded environment
*	where multiple tasks may try modifying the list concurrently.
*
*	@param	list	Pointer to the singly linked list where task will be added.
*								The list must be valid and initialised.
*	@param	task	Pointer to the TCB which will be added to the list.
*								The task must not be NULL.
*
*	1.	Enters a do...while loop which will continue until the task is
*			successfully added to the list.
*	2.	Inside the loop, it uses the LDREX to load the current
*			head of the list. LDREX will also set the exclusive flag, which is crucial
*			and will be checked later by the STREX.
*	3.	Sets the 'next' pointer of the new task to the current head of the list.
*	4.	STREX attempts to store the new task as the new head of list atomically.
*			If the exclusive flag set by LDREX is still valid, STREX will succeed (returns 0)
*			and the loop will exit.
*			Otherwise, the exclusive flag is invalid, meaning another task modified the list,
*			STREX fails (returns 1) and the loop repeats itself until success.
*/
void list_push_sl(OS_tasklist_t *list, OS_TCB_t *task) {
	do {
		OS_TCB_t *head = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		task->next = head;
	}
	while (__STREXW ((uint32_t) task, (uint32_t volatile *)&(list->head)));
}

/**
*	Atomically pops the head of a singly linked list.
*	Removes and returns the first (head) TCB from the list in a thread safe manner,
*	ensuring consistent behaviour even when multiple tasks try to modify the list
*	concurrently.
*
*	@param	list	Pointer to singly linked list from which the TCB will be removed.
*								The list must be valid and initialised.
*	@return				Pointer to the popped TCB if the list isn't empty, otherwise NULL.
*
*	1.	Enters a do...while which will continue until the task is removed from the list.
*	2.	Inside the loop, it uses LDREX to atomically load the current head of the list
*			and set it as 'oldHead'. Does this as well as setting the exclusive flag.
*	3.	If 'oldHead' is NULL (the list is empty), it clears the exclusive flag using CLREX.
*			Returns NULL.
*	4.	If 'oldHead' isn't NULL, function uses STREX to atomically update the head of the list
*			to the next element. If exclusive flag is valid, STREX will succeed and the loop will exit.
*			Otherwise, STREX will fail and the loop will repeat itself. 
*	5.	Once the loop exits, 'oldHead' is returned, meaning the task is popped from the list.
*/
OS_TCB_t* list_pop_sl(OS_tasklist_t *list) {
	OS_TCB_t * oldHead = 0;
	do {
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		if (!oldHead) {
			__CLREX();
			return 0;
		}
	}
	while (__STREXW ((uint32_t) oldHead->next, (uint32_t volatile *)&(list->head)));
	return oldHead;
}

/**
*	Atomically pops the tail of a singly linked list.
*	Removes and returns the last TCB from the list in a thread safe manner. Uses atomic
*	operations to ensure safety in a multi threaded environment.
*
*	@param	list	Pointer to the singly linked list from which the TCB will be removed.
*								The list must be valid and initialised.
*	@return				Pointer to the popped tail TCB if the list isn't empty. Otherwise NULL.
*
*	1.	Enters a loop trying to atomically find and remove the tail of the list.
*	2.	Inside the loop, uses LDREX to atomically load the current head of list, as well
*			as setting the exclusive flag.
*	3.	If the list is NULL (empty), function exits and returns NULL.
*	4.	If there is only one element in the list, attempts to atomically set the head of
*			the list to NULL using STREX. If successful, returns the single item.
*			Otherwise, STREX fails and retries the operation.
*	5.	If the list contains more than one element, finds the second to last element (current) in
*			the list, since a tail cannot be easily found in a single linked list.
*	6.	Removes the tail from the list (current->next) by setting it to NULL.
*	7.	The tail node is returned.
*/
OS_TCB_t* list_pop_tail_sl(OS_tasklist_t *list) {
	OS_TCB_t *oldHead = 0;
	OS_TCB_t *current = 0;
	do {
		oldHead = (OS_TCB_t *) __LDREXW ((uint32_t volatile *)&(list->head));
		if (!oldHead) {
			return 0;
		}
		if (!oldHead->next) {
			if (__STREXW((uint32_t)0, (uint32_t volatile *)&(list->head)) == 0) {
				return oldHead;
			}
			__CLREX();
			continue;
		}
		current = oldHead;
		while (current->next && current->next->next) {
			current = current->next;
		}
	}
	while (__STREXW((uint32_t)oldHead, (uint32_t volatile *)&(list->head)));
	OS_TCB_t *tail = current->next;
	current->next = 0;
	return tail;
}

/**
*	Fixed Priority & Round Robin scheduler.
*	Main driver of the OS. Selects the next task to run based on a fixed-priority,
*	as well as incorporating round-robin within each priority level. It also processes
* tasks in the sleep and pending lists, moving them back to the scheduler when they are
* ready to run.
*
*	@return		Pointer to the next TCB to run.
*
*	1.	Processes the sleep list:
*			-	Wakes up TCBs whose sleep time has expired
*			-	Moves awakened tasks to the pending list for further processing
*
*	2.	Processes the pending list:
*			-	Moves tasks from pending list back to their respective priority queue.
*			-	Ensures tasks are now considered for scheduling again, if possible.
*
*	3.	Implements fixed priority scheduling with round-robin
*			-	Iterates through each priority queue, from highest to lowest.
*			-	For each priority level, if it's not empty, selects the next task after
*				applying round robin to the queue
*			-	Ensures fairness among tasks of the same priority level
*			-	Returns the next task to run
*
*	4.	In case no runnable tasks are found in any of the queues, returns the idle task.
*			-	The idle task is a special task, which runs indefinitely only when no other tasks are runnable.
*			-	This ensures the CPU is not left idle when there are no tasks to execute.
*/
OS_TCB_t const * _OS_schedule(void) {
	// Wake up tasks in the sleep list if wake up time has elapsed
	while (sleep_list.head && (sleep_list.head->data <= OS_elapsedTicks())) {
		OS_TCB_t *taskToWake = list_pop_sl(&sleep_list);
		taskToWake->state &= ~TASK_STATE_SLEEP;
		list_push_sl(&pending_list, taskToWake);
	}
	
	// Move tasks from pending list to correct priority queue
	while (pending_list.head) {
		OS_TCB_t *taskToProcess = list_pop_sl(&pending_list);
		list_add(&task_queues[taskToProcess->priority], taskToProcess);
	}
	
	// Iterate through all priority levels, starting from the highest
	for (int8_t prio = MAX_TASK_PRIORITY_LEVELS - 1; prio >= 0; prio--) {
		// Implement round-robin within this priority level
		if (task_queues[prio].head) {
			// Round-robin within the priority level
			task_queues[prio].head = task_queues[prio].head->next;
			task_queues[prio].head->state &= ~TASK_STATE_YIELD;
			return task_queues[prio].head;
		}
	}
	// Return the idle task if no other tasks are runnable
	return _OS_idleTCB_p;
}

/* Initialises a task control block (TCB) and its associated stack.  See os.h for details. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data, uint32_t priority) {
	TCB->sp = stack - (sizeof(_OS_StackFrame_t) / sizeof(uint32_t));
	TCB->state = 0;
	TCB->prev = TCB->next = 0;
	TCB->priority = priority;
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
	if (tcb->priority < MAX_TASK_PRIORITY_LEVELS) {
		list_add(&task_queues[tcb->priority], tcb);
	}
}

/* SVC handler that's called by _OS_task_end when a task finishes.  Removes the
   task from the scheduler and then queues PendSV to reschedule. */
void _OS_taskExit_delegate(void) {
	// Remove the given TCB from the list of tasks so it won't be run again
	OS_TCB_t * tcb = OS_currentTCB();
	// Check if the task's priority is within the valid range
	if (tcb->priority < MAX_TASK_PRIORITY_LEVELS) {
		// Remove the task from its priority queue
		list_remove(&task_queues[tcb->priority], tcb);
	}
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
