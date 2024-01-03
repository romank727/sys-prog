#include "OS/mutex.h"
#include "OS/semaphore.h"
#include "OS/mempool.h"
#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>

static OS_mutex_t mutex = OS_MUTEX_STATIC_INITIALISER;
static OS_semaphore_t semaphore = OS_SEMAPHORE_STATIC_INITIALISER;

/*========================*/
/*     TASK DEFINITION   	*/
/*			DO NOT TOUCH!			*/
/*========================*/

/**
*	TaskFunction_t: A pointer to a function that represents a task
*									in the system.
*
*	This function pointer is designed to point to functios which
*	conform to a specific format, where the function returns void and accepts
*	a single pointer argument. 
*	In other words, this allows for a universal way of defining tasks
*	throughout the system.
*/
typedef void (*TaskFunction_t)(void const * const args);

/**
*	TaskDef_t: Structure representing the defition of a task.
*
*	This struct contains all the essential information that's needed
*	to create and initialise some task.
*
*	@field	taskFunc		Pointer to the function that the task will execute.
*
*	@field	priority		The priority level of the task. Tasks with higher
*											numerical values are of higher priority.
*
*	@field	stackSize		The size of the stack allocated to this task. The size
*											of this will be cast to 'uint32_t'.
*/
typedef struct {
    TaskFunction_t taskFunc;
    uint32_t priority;
    size_t stackSize;
} TaskDef_t;

/**
*	Createse a new task and adds it to the scheduler.
*
*	@param	taskDef		Pointer to the structure containing task details like
*										function, priority and stack size.
*
*	@param	args			Pointer to the arguments that might be passed to the
*										task function.
*
*	1.	Allocates memory for the task's stack from a static memory pool, using
*			the stack size specified in the task definition.
*				-	This retains 8 byte alignment due to how 'static_alloc' works.
*	2.	Allocates memory for the task's TCB.
*	3.	Initialises the TCB with the allocated stack, function, arguments and priority.
*	4.	Adds the initialised task to the scheduler for execution.
*/
void OS_createTask(TaskDef_t *taskDef, void const * const args);
void OS_createTask(TaskDef_t *taskDef, void const * const args) {
    // Allocate memory for the task's stack
    uint32_t *stack = (uint32_t *)static_alloc(taskDef->stackSize * sizeof(uint32_t));
    // Allocate memory for the TCB
    OS_TCB_t *tcb = (OS_TCB_t *)static_alloc(sizeof(OS_TCB_t));
    // Initialise the TCB with the provided stack and task details
    OS_initialiseTCB(tcb, stack + taskDef->stackSize, taskDef->taskFunc, args, taskDef->priority);
    // Add the task to the scheduler
    OS_addTask(tcb);
}

/*=============================*/
/*     	USER CONFIGURABLE   	 */
/*			TASK IMPLEMENTATIONS	 */
/*=============================*/

/*	Declare and initialise different tasks	*/
__attribute__((noreturn))
static void task1(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_semaphore_acquire(&semaphore);
		printf("AAAAAAAA\r\n");
		OS_semaphore_release(&semaphore);
	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_semaphore_acquire(&semaphore);
		printf("BBBBBBBB\r\n");
		OS_semaphore_release(&semaphore);
	}
}

__attribute__((noreturn))
static void task3(void const *const args) {
	(void) args;
	while (1) {
		OS_sleep(300);
		OS_mutex_acquire(&mutex);
		//OS_semaphore_acquire(&semaphore);
		printf("CCCCCCCC\r\n");
		//OS_semaphore_release(&semaphore);
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task4(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(100);
		OS_mutex_acquire(&mutex);
		printf("DDDDDDDD\r\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task5(void const *const args) {
	(void) args;
	while (1) {
		OS_sleep(150);
		OS_mutex_acquire(&mutex);
		printf("EEEEEEEE\r\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task6(void const *const args) {
	(void) args;
	while (1) {
		
		void *block1 = pool_allocate_from(0);
		if (block1) {
			printf("First block allocated at address %p\n", block1);
			//pool_deallocate_to(0, block1); // Deallocate back to pool 0
		}

		// Allocate another block from the same pool
		void *block2 = pool_allocate_from(0);
		if (block2) {
			printf("Second block allocated at address %p\n", block2);
			//pool_deallocate_to(0, block2);
		}
		
	}
}

/* MAIN FUNCTION */

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("\r\nDocetOS\r\n");

	/*	Configure tasks	*/
	
	TaskDef_t task1Def = {
		.taskFunc = task1,
		.priority = 4,
		.stackSize = 128
	};
	
	TaskDef_t task2Def = {
		.taskFunc = task2,
		.priority = 4,
		.stackSize = 128
	};
	
	TaskDef_t task3Def = {
		.taskFunc = task3,
		.priority = 4,
		.stackSize = 128
	};
	
	TaskDef_t task4Def = {
		.taskFunc = task4,
		.priority = 3,
		.stackSize = 128
	};
	
	TaskDef_t task5Def = {
		.taskFunc = task5,
		.priority = 3,
		.stackSize = 128
	};
	
	TaskDef_t task6Def = {
		.taskFunc = task6,
		.priority = 1,
		.stackSize = 128
	};
	
	/*	Add the tasks to the scheduler	*/
	
	OS_createTask(&task1Def, NULL);
	//OS_createTask(&task2Def, NULL);
	//OS_createTask(&task3Def, NULL);
	//OS_createTask(&task4Def, NULL);
	//OS_createTask(&task5Def, NULL);
	//OS_createTask(&task6Def, NULL);
	
	/*	Initialise user's configured memory pools	*/
	mempools_init();
	
	/* Start the OS */
	OS_start();
}
