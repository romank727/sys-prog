#include "OS/mutex.h"
#include "OS/os.h"
#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>

static OS_mutex_t mutex = OS_MUTEX_STATIC_INITIALISER;

__attribute__((noreturn))
static void task1(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("AAAAAAAA\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("BBBBBBBB\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task3(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("CCCCCCCC\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task4(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("DDDDDDDD\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task5(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("EEEEEEEE\n");
		OS_mutex_release(&mutex);
	}
}

/* MAIN FUNCTION */

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("\r\nDocetOS\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	static uint32_t stack1[128] __attribute__ (( aligned(8) ));
	static uint32_t stack2[128] __attribute__ (( aligned(8) ));
	static uint32_t stack3[128] __attribute__ (( aligned(8) ));
	static uint32_t stack4[128] __attribute__ (( aligned(8) ));
	static uint32_t stack5[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5;

	/* Initialise the TCBs using the two functions above */
	
	OS_initialiseTCB(&TCB1, stack1+128, task1, NULL);
	OS_initialiseTCB(&TCB2, stack2+128, task2, NULL);
	OS_initialiseTCB(&TCB3, stack3+128, task3, NULL);
	OS_initialiseTCB(&TCB4, stack4+128, task4, NULL);
	OS_initialiseTCB(&TCB5, stack5+128, task5, NULL);
	
	/* Add the tasks to the scheduler */
	
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	OS_addTask(&TCB5);
	
	/* Start the OS */
	
	OS_start();
	
}
/*
	https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/bd0f87c18b292f45ebf4d4b4c5bb96765465f760/examples/sample_configuration/FreeRTOSConfig.h
	https://www.freertos.org/a00110.html
	https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/bd0f87c18b292f45ebf4d4b4c5bb96765465f760/portable/Tasking/ARM_CM4F/port_asm.asm
	https://www.freertos.org/RTOS-task-priority.html
	https://www.freertos.org/fr-content-src/uploads/2018/07/FreeRTOS_Reference_Manual_V10.0.0.pdf
*/
