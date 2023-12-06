#ifndef SLEEP_H
#define SLEEP_H

#define OS_INTERNAL

#include "stm32f4xx.h"

// definition of this is in "os.h", therefore external
extern uint32_t OS_elapsedTicks(void);

void sort_sleep_list(void * taskToInsert);

#endif /* SLEEP_H */
