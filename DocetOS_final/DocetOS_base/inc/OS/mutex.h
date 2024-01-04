#ifndef MUTEX_H
#define MUTEX_H

#include "OS/os.h"

/** 
* Static initializer for a mutex. It sets the mutex with no task
* holding it and an empty waiting list.
*/
#define OS_MUTEX_STATIC_INITIALISER {.taskBlock = 0, .counter = 0, .waitingList = {.head = 0}}

/**
* Structure representing a mutex.
* The mutex is used for managing access to shared resources in a multitasking environment.
*/
typedef struct s_OS_mutex_t {
	/**
	*	Pointer to the TCB of the task that currently holds the mutex.
	*	Marked volatile to ensure proper behavior in a multi-threaded environment.
	*/
	OS_TCB_t * volatile taskBlock;
	
	/**
	* Counter to track the number of times the mutex has been acquired by the same task.
	* This is used to implement a re-entrant mutex, where the same task can lock the mutex
	* multiple times without causing a deadlock.
	*/
	uint32_t counter;
	
	/**
	* List of tasks waiting for this mutex to become available. Tasks in this list are
	* blocked from executing until the mutex is released by its current holder.
	*/
	OS_tasklist_t waitingList;
} OS_mutex_t;

/**
* Acquires a mutex. If the mutex is already held by another task, the calling task will
* be blocked and added to the mutex's waiting list until the mutex becomes available.
*
* @param	mutex		Pointer to the mutex to be acquired.
*/
void OS_mutex_acquire (OS_mutex_t * mutex);

/**
* Releases a mutex. If the current task holds the mutex, this function will decrement the 
* lock counter and potentially release the mutex if the count reaches zero. This allows
* other tasks waiting for the mutex to acquire it.
*
* @param	mutex		Pointer to the mutex to be released.
*/
void OS_mutex_release (OS_mutex_t * mutex);

/**
* Notifies a task that the mutex is available. This function removes 
* a task from the mutex's waiting list and adds it to the pending list, making it ready 
* for execution.
*
* @param	mutex		Pointer to the mutex for which the task is being notified.
*/
void OS_notify(OS_mutex_t *mutex);

#endif /* MUTEX_H */
