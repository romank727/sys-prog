#ifndef MUTEX_H
#define MUTEX_H

#define OS_INTERNAL
#include "OS/os.h"

typedef struct s_OS_mutex_t {
	OS_TCB_t * volatile taskBlock;
	uint32_t counter;
} OS_mutex_t;

#define OS_MUTEX_STATIC_INITIALISER {.taskBlock = 0, .counter = 0}

void OS_mutex_acquire (OS_mutex_t * mutex);
void OS_mutex_release (OS_mutex_t * mutex);

#endif /* MUTEX_H */
