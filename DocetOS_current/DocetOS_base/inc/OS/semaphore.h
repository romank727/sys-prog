#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "OS/os.h"
#include "stm32f4xx.h"

typedef struct s_OS_semaphore_t {
	volatile uint32_t token; // 0 if available, 1 if taken
	_OS_tasklist_t waitingList;
} OS_semaphore_t;

#define OS_SEMAPHORE_STATIC_INITIALISER {.token = 0, .waitingList = {.head = 0}}

void OS_semaphore_acquire(OS_semaphore_t * semaphore);
void OS_semaphore_release(OS_semaphore_t * semaphore);

#endif /* SEMAPHORE_H */
