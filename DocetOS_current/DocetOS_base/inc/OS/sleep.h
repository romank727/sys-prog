#ifndef SLEEP_H
#define SLEEP_H

#define OS_INTERNAL

#include "OS/os.h"
#include "stm32f4xx.h"
#include "OS/scheduler.h"

extern _OS_tasklist_t task_list;
extern _OS_tasklist_t sleep_list;

void sort_sleep_list(OS_TCB_t *insertTask);
void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack);

#endif /* SLEEP_H */
