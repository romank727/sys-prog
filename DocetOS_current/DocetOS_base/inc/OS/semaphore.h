#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "OS/os.h"

/**
* Static initialiser for the binary semaphore.
* Initialises the semaphore in an unlocked state with no waiting tasks.
*/
#define OS_SEMAPHORE_STATIC_INITIALISER {.token = 0, .waitingList = {.head = 0}}

/**
* Structure for a binary semaphore.
* It contains a token to indicate the semaphore's state and a list for tasks waiting for the semaphore.
*/
typedef struct s_OS_semaphore_t {
	uint32_t volatile token; // 0 if available, 1 if taken
	OS_tasklist_t waitingList;
} OS_semaphore_t;

/**
* Attempts to acquire a semaphore. If the semaphore is already taken, the calling task will be blocked
* until the semaphore becomes available.
*
* @param	semaphore		Pointer to the semaphore to be acquired.
*/
void OS_semaphore_acquire(OS_semaphore_t * semaphore);

/**
* Releases a semaphore that was previously acquired. If there are tasks waiting on the semaphore,
* they are moved to a runnable state.
*
* @param	semaphore		Pointer to the semaphore to be released.
*/
void OS_semaphore_release(OS_semaphore_t * semaphore);

#endif /* SEMAPHORE_H */
