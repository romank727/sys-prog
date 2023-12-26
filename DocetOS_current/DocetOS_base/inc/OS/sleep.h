#ifndef SLEEP_H
#define SLEEP_H

#include "stm32f4xx.h"

extern uint32_t OS_elapsedTicks(void);

void i_sort_sleep_list(void * taskToInsert);

#endif /* SLEEP_H */
