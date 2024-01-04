#include "OS/mutex.h"
#include "OS/semaphore.h"
#include "OS/mempool.h"
#include "OS/demo_prototypes.h"
#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>

// A global counter used for demonstration code purposes.
static uint32_t demoCounter = 0;

/**
*	Static declarations of a mutex and semaphore, which will be
*	utilised in tasks.
*/
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
*	This function pointer is designed to point to functions which
*	conform to a specific format, where the function returns void and accepts
*	a single pointer argument. 
*	In other words, this allows for a universal way of defining tasks
*	throughout the system.
*/
typedef void (*TaskFunction_t)(void const * const args);

/**
*	TaskDef_t: Structure representing the definition of a task.
*
*	This struct contains all the essential information that's needed
*	to create and initialise a task.
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

/** DEMO NOTE **
*	The following order of tasks is the expected order they should
* be executed in the demonstration.
*
*	In other words, memory pool tasks should run first because of 
*	the highest priority, followed by mutex tasks, semaphore and finally
*	the sleep tasks.
*	
*	This is a clear demonstration of the fixed-priority scheduler 
*	working correctly.
*	So if the priorities have been left unchanged, the current 
*	correct order of execution should be:
*		1.	Memory pool tasks
*		2.	Mutex tasks
*		3.	Binary semaphore tasks
*		4.	Sleeping tasks
*/

static	TaskDef_t memPoolTask = {
					.taskFunc = memPool_demoTask,
					.priority = 4,
					.stackSize = 128
				};

static TaskDef_t mutexTasks[] = {
    { .taskFunc = mutex_demoTask1, .priority = 3, .stackSize = 128 },
    { .taskFunc = mutex_demoTask2, .priority = 3, .stackSize = 128 },
    { .taskFunc = mutex_demoTask3, .priority = 3, .stackSize = 128 }
};

static TaskDef_t binarySemaphoreTasks[] = {
    { .taskFunc = binarySemaphore_demoTask1, .priority = 2, .stackSize = 128 },
    { .taskFunc = binarySemaphore_demoTask2, .priority = 2, .stackSize = 128 },
    { .taskFunc = binarySemaphore_demoTask3, .priority = 2, .stackSize = 128 }
};

static TaskDef_t sleepTasks[] = {
    { .taskFunc = sleep_demoTask1, .priority = 1, .stackSize = 128 },
    { .taskFunc = sleep_demoTask2, .priority = 1, .stackSize = 128 },
    { .taskFunc = sleep_demoTask3, .priority = 1, .stackSize = 128 }
};

/* MAIN FUNCTION */
int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("\r\nDocetOS Demo\r\n\n");
	
	/*	Initialise user's configured memory pools	*/
	mempools_init();
	
	/*	Schedule tasks	*/
	for (uint32_t i = 0; i < sizeof(sleepTasks) / sizeof(sleepTasks[0]); i++) {
		OS_createTask(&sleepTasks[i], NULL);
	}
	
	for (uint32_t i = 0; i < sizeof(mutexTasks) / sizeof(mutexTasks[0]); i++) {
		OS_createTask(&mutexTasks[i], NULL);
	}
	
	for (uint32_t i = 0; i < sizeof(binarySemaphoreTasks) / sizeof(binarySemaphoreTasks[0]); i++) {
		OS_createTask(&binarySemaphoreTasks[i], NULL);
	}
	
	OS_createTask(&memPoolTask, NULL);
	
	/* Start the OS */
	OS_start();
}

/**
*	A task to demonstrate a working memory pool implementation.
*
*	The demonstration only uses pool 2, but can be easily changed to use
*	any other pool as needed. 
*
*	In the demonstration, the task allocates the first block at some address,
*	after which it deallocates it. This means that address should now be 
*	free to be used, so the second block should be allocated at said address.
*/
void memPool_demoTask(void const *const args) {
	(void) args;
	if (demoCounter == 0) {
		printf("\r\nMemory Pool Tasks\r\n\n");
		demoCounter++;
	}
	for (uint32_t i = 0; i < 5; i++) {
		void *block1 = pool_allocate_from(2);
		if (block1) {
			printf("First block allocated at %p\r\n", block1);
			pool_deallocate_to(2, block1); // Deallocate back to pool 2
		}
		printf("First block deallocated\r\n");
		
		// Allocate another block from the same pool
		void *block2 = pool_allocate_from(2);
		if (block2) {
			printf("Second block allocated at %p\r\n\n", block2);
		}
	}
}

/**
*	3 tasks to demonstrate a working re-entrant mutex. 
*	
*	Each task should be able to output its own string of letters,
*	without the serial getting cluttered. 
*
*	Due to how my scheduler is implemented, the expected output
*	should be:
*							BBBBBBBB
*							CCCCCCCC
*							AAAAAAAA
*/
void mutex_demoTask1(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_mutex_acquire(&mutex);
		printf("AAAAAAAA\r\n\n");
		OS_mutex_release(&mutex);
	}
}

void mutex_demoTask2(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_mutex_acquire(&mutex);
		if (demoCounter == 1) {
			printf("Mutex Tasks\r\n\n");
			demoCounter++;
		}
		printf("BBBBBBBB\r\n");
		OS_mutex_release(&mutex);
	}
}

void mutex_demoTask3(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_mutex_acquire(&mutex);
		printf("CCCCCCCC\r\n");
		OS_mutex_release(&mutex);
	}
}

/**
*	3 tasks to demonstrate a binary semaphore implementation.
*	
*	The output should be similar to the mutex and the serial
*	must clearly output each string belonging to each task.
*
*	Due to how my scheduler is implemented, the expected output
*	should be:
*							YYYYYYYY
*							ZZZZZZZZ
*							XXXXXXXX
*/
void binarySemaphore_demoTask1(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_semaphore_acquire(&semaphore);
		printf("XXXXXXXX\r\n\n");
		OS_semaphore_release(&semaphore);
	}
}

void binarySemaphore_demoTask2(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_semaphore_acquire(&semaphore);
		if (demoCounter == 2) {
			printf("Binary Semaphore Tasks\r\n\n");
			demoCounter++;
		}
		printf("YYYYYYYY\r\n");
		OS_semaphore_release(&semaphore);
	}
}

void binarySemaphore_demoTask3(void const *const args) {
	(void) args;
	for (uint32_t i = 0; i < 5; i++) {
		OS_semaphore_acquire(&semaphore);
		printf("ZZZZZZZZ\r\n");
		OS_semaphore_release(&semaphore);
	}
}

/**
*	3 tasks to demonstrate the sleep mechanism implementation.
*
*	Each of the tasks have a different sleep timer to try and
*	emphasize on the correct wake-up time.
*	
*	The expected output should be:
*		-	3x task 3
*		-	2x task 2
*		-	1x task 1
*
*	Note: Serial clogging might be observed every once in a while,
*				since I'm explicitly just trying to show the sleeping
*				mechanism in action.
*				A mutex or a semaphore would fix this.
*/
__attribute__((noreturn))
void sleep_demoTask1(void const *const args) {
	(void) args;
	while (1) {
		OS_sleep(3000);
		printf("Sleepy task 1\r\n\n");
	}
}

__attribute__((noreturn))
void sleep_demoTask2(void const *const args) {
	(void) args;
	while (1) {
		if (demoCounter == 3) {
			printf("Sleep Tasks\r\n\n");
			demoCounter++;
		}
		OS_sleep(1500);
		printf("Sleepy task 2\r\n");
	}
}

__attribute__((noreturn))
void sleep_demoTask3(void const *const args) {
	(void) args;
	while (1) {
		OS_sleep(1000);
		printf("Sleepy task 3\r\n");
	}
}
