#ifndef MUTEX_H
#define MUTEX_H

#define OS_INTERNAL

#include "OS/os.h"
#include "stm32f4xx.h"

typedef struct s_OS_mutex_t {
	// pointer to the TCB of the task that currently holds the mutex.
  // volatile to ensure proper behavior in a multi-threaded environment.
	OS_TCB_t * volatile taskBlock;
	uint32_t counter;
	_OS_tasklist_t waitingList;
} OS_mutex_t;

#define OS_MUTEX_STATIC_INITIALISER {.taskBlock = 0, .counter = 0, .waitingList = {0}}

// Acquire a mutex. If the mutex is already held, the calling task will be blocked
// until the mutex becomes available.
void OS_mutex_acquire (OS_mutex_t * mutex);

// Release a mutex. If the current task holds the mutex, this will decrement the lock
// counter and potentially release the mutex if it reaches zero, allowing other tasks
// to acquire it.
void OS_mutex_release (OS_mutex_t * mutex);

void OS_notify(OS_mutex_t *mutex);

#endif /* MUTEX_H */
