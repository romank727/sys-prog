#ifndef __scheduler_h__
#define __scheduler_h__

#include "OS/config.h"
#include <stdint.h>

/*========================*/
/*      EXTERNAL API      */
/*========================*/

/**
*	Represents a Task Control Block (TCB) in the OS.
*	Stores essential details necessary for task scheduling, state managing,
*	context switching and such.
*
*		-	sp:						Stack Pointer for the task. Volatile to ensure safety.
*										The first entry in this structure ensures easy access to the stack
*										pointer during context switches.
*
*		-	state:				A volatile integer representing the current state of the task.
*										This includes states like ready, running, asleep etc.
*										Current defined states:
*											- TASK_STATE_YIELD (bit 0)
*											-	TASK_STATE_SLEEP (bit 1)
*
*		-	data:					Can hold additional data related to the task. Specific
*										usage can vary and might be used for different purposes.
*										e.g. Storing wake-up time.
*
*		-	prev,	next:		Pointers to the previous and next TCBs in a linked list. Used
*										to chain tasks together in various task lists or queues managed by the OS.
*
*		-	priority:			An integer representing the priority of the task. Tasks with higher priority
*										values are always given preference by the scheduler, unless they are currently
*										in a sleeping state.
*/
typedef struct s_OS_TCB_t {
	void * volatile sp;
	uint32_t volatile state;
	uint32_t volatile data;
	struct s_OS_TCB_t * prev;
	struct s_OS_TCB_t * next;
	uint32_t priority;
} OS_TCB_t;

/**
*	Represents a list of tasks in the OS.
*	This struct is used to manage a collection of TCBs, usually
*	for the purpose of task scheduling and other similar management.
*	Currently serves a vital part in the OS, allowing various task related
*	mechanisms like the priority queues and other lists.
*
*		- head: A pointer to the first TCB in the list. Marked as volatile to
*						ensure correct behaviour in the presence of interrupts and other
*						concurrent accesses.
*						In other words, used as a starting point for traversing the list or for
*						adding and removing tasks.
*/
typedef struct {
	OS_TCB_t * volatile head;
} OS_tasklist_t;

/******************************************/
/* Task creation and management functions */
/******************************************/

/**	
*	Initialises a task control block (TCB) and its associated stack.  The stack is prepared
*	with a frame such that when this TCB is first used in a context switch, the given function will be
*	executed.	If and when the function exits, a SVC call will be issued to kill the task, and a callback
*	will be executed.
*		- The first argument is a pointer to a TCB structure to initialise.
*		- The second argument is a pointer to the TOP OF a region of memory to be used as a stack (stacks are full descending).
*			Note that the stack MUST be 8-byte aligned.  This means if (for example) malloc() is used to create a stack,
*			the result must be checked for alignment, and then the stack size must be added to the pointer for passing
*			to this function.
*		- The third argument is a pointer to the function that the task should execute.
*		- The fourth argument is a void pointer to data that the task should receive. 
*		- The fifth argument is an integer storing the priority of set task. 
*/
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data, uint32_t priority);

/**
*	Schedules a user-defined task by adding it to the appropriate task queue based on its priority.
*	This function is used to integrate a task into the system's scheduling mechanism.
*
* @param	tcb		Pointer to the Task Control Block (TCB) of the task to be scheduled.
*            		The TCB should be properly initialised with a set priority, stack pointer,
*            		and initial stack frame. The priority determines the queue in which
*            		the task will be placed for scheduling.
*/
void OS_addTask(OS_TCB_t * const tcb);

/*========================*/
/*      INTERNAL API      */
/*========================*/

#ifdef OS_INTERNAL

OS_TCB_t const * _OS_schedule(void);

/*	Extern declarations for task list variables used in the scheduler */

//	Task queues for each priority level, holding tasks ready to run
extern OS_tasklist_t task_queues[MAX_TASK_PRIORITY_LEVELS];
//	List for tasks transitioning between states or queues
extern OS_tasklist_t pending_list;
//	List of tasks currently asleep
extern OS_tasklist_t sleep_list;

/**
* Adds a task control block (TCB) to a doubly linked list.
*	@param	list	Pointer to the task list to which the TCB is to be added.
* @param	task	Pointer to the TCB to be added to the list.
*/
void list_add(OS_tasklist_t *list, OS_TCB_t *task);

/**
* Removes a task control block (TCB) from a specified doubly linked list.
* @param	list	Pointer to the task list from which the TCB is to be removed.
* @param	task	Pointer to the TCB to be removed from the list.
*/
void list_remove(OS_tasklist_t *list, OS_TCB_t *task);

/**
* Pushes a task control block (TCB) to the head of a singly linked list.
* This operation is attempted to be done atomically.
*	@param	list	Pointer to the singly linked task list.
* @param	task	Pointer to the TCB to be pushed to the list.
*/
void list_push_sl(OS_tasklist_t *list, OS_TCB_t *task);

/**
* Pops and returns the head task control block (TCB) from a singly linked list.
* This operation is attempted to be done atomically.
* @param	list	Pointer to the singly linked task list.
* @return				Pointer to the popped TCB, or NULL if the list is empty.
*/
OS_TCB_t* list_pop_sl(OS_tasklist_t *list);

/**
* Pops and returns the tail task control block (TCB) from a singly linked list.
* This operation is attempted to be done atomically.
* @param	list	Pointer to the singly linked task list.
* @return				Pointer to the popped TCB, or NULL if the list is empty.
*/
OS_TCB_t* list_pop_tail_sl(OS_tasklist_t *list);

/* SVC delegates */
void _OS_taskExit_delegate(void);

/* Constants that define bits in a thread's 'state' field. */
#define TASK_STATE_YIELD    (1UL << 0) // Bit zero is the 'yield' flag
#define TASK_STATE_SLEEP    (1UL << 1) // Bit one is the 'sleep' flag


#endif /* os_internal */

#endif /* __scheduler_h__ */
