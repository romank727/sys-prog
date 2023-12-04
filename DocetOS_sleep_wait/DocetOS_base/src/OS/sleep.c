#define OS_INTERNAL

#include "OS/sleep.h"
#include "OS/os.h"
#include "stm32f4xx.h"
#include "OS/scheduler.h"

//void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack);
//void _OS_sleep_delegate(_OS_SVC_StackFrame_t *svcStack) {
//	// _OS_SVC_StackFrame_t *svcStack = (_OS_SVC_StackFrame_t *)stack;
//	// current task's TCB
//	OS_TCB_t *currentTask = OS_currentTCB();
//	// calculate when the task would need to wake up 
//	uint32_t wakeUp = OS_elapsedTicks() + svcStack->r0;
//	// store the wake up time in the TCB
//	currentTask->data = wakeUp;
//	// indicate the current task state to be sleeping
//	currentTask->state |= TASK_STATE_SLEEP;
//	
//	list_remove(&task_list, currentTask);
//	sort_sleep_list(currentTask);
//	
//	// setting the PendSV bit to trigger a context switch
//	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
//}
