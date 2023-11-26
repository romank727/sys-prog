#define OS_INTERNAL

#include "OS/sleep.h"
#include "OS/os.h"
#include <stdint.h>

void OS_sleep(uint32_t waitNum) {
	// current task's TCB
	OS_TCB_t *currentTCB = OS_currentTCB();
	// calculate when the task would need to wake up 
	uint32_t wakeUp = OS_elapsedTicks() + waitNum;
	// store the wake up time in the TCB
	currentTCB->data = &wakeUp;
	// indicate the current task state to be sleeping
	currentTCB->state |= TASK_STATE_SLEEP;
	
	OS_yield();

}
