#ifndef __scheduler_h__
#define __scheduler_h__

#include <stdint.h>

/*========================*/
/*      EXTERNAL API      */
/*========================*/

typedef struct s_OS_TCB_t {
	/* Task stack pointer.  It's important that this is the first entry in the structure,
	   so that a simple double-dereference of a TCB pointer yields a stack pointer. */
	void * volatile sp;
	/* This field is intended to describe the state of the thread - whether it's yielding,
	   runnable, or whatever.  Only one bit of this field is currently defined (see the #define
	   below), so you can use the remaining 31 bits for anything you like. */
	uint32_t volatile state;
	/* This field acts as a integer data type.
	   In this case, bit 1 of the 31 bits in use represents the data. */
	/* bit 0 = state
		 bit 1 = sleep */
	uint32_t volatile data;
	struct s_OS_TCB_t * prev;
	struct s_OS_TCB_t * next;
} OS_TCB_t;


/******************************************/
/* Task creation and management functions */
/******************************************/

/* Initialises a task control block (TCB) and its associated stack.  The stack is prepared
   with a frame such that when this TCB is first used in a context switch, the given function will be
   executed.  If and when the function exits, a SVC call will be issued to kill the task, and a callback
   will be executed.
   The first argument is a pointer to a TCB structure to initialise.
   The second argument is a pointer to the TOP OF a region of memory to be used as a stack (stacks are full descending).
     Note that the stack MUST be 8-byte aligned.  This means if (for example) malloc() is used to create a stack,
     the result must be checked for alignment, and then the stack size must be added to the pointer for passing
     to this function.
   The third argument is a pointer to the function that the task should execute.
   The fourth argument is a void pointer to data that the task should receive. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data);

void OS_addTask(OS_TCB_t * const tcb);

void OS_notifyAll(void);

uint32_t notification_counter(void);

/*========================*/
/*      INTERNAL API      */
/*========================*/

#ifdef OS_INTERNAL

OS_TCB_t const * _OS_schedule(void);

typedef struct {
	OS_TCB_t * volatile head;
} _OS_tasklist_t;

extern _OS_tasklist_t task_list;
extern _OS_tasklist_t sleep_list;

void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task);
void list_remove(_OS_tasklist_t *list, OS_TCB_t *task);

/* SVC delegates */
void _OS_taskExit_delegate(void);

/* Constants that define bits in a thread's 'state' field. */
#define TASK_STATE_YIELD    (1UL << 0) // Bit zero is the 'yield' flag
#define TASK_STATE_SLEEP    (1UL << 1) // Bit one is the 'sleep' flag


#endif /* os_internal */

#endif /* __scheduler_h__ */
