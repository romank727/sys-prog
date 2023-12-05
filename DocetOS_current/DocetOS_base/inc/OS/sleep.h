#ifndef SLEEP_H
#define SLEEP_H

#define OS_INTERNAL

#include "OS/os.h"
#include "stm32f4xx.h"
#include "OS/scheduler.h"

// definition of these is in "scheduler.c and .h", therefore external
extern _OS_tasklist_t task_list;
extern _OS_tasklist_t sleep_list;
extern uint32_t OS_elapsedTicks(void);
extern void list_push_sl(_OS_tasklist_t *list, OS_TCB_t *task);
extern void list_remove(_OS_tasklist_t *list, OS_TCB_t *task);

void sort_sleep_list(OS_TCB_t *insertTask);
void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack);

#endif /* SLEEP_H */
