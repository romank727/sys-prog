#include "OS/mutex.h"
#include "OS/semaphore.h"
#include "OS/mempool.h"
#include "OS/os.h"
#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>

static OS_mutex_t mutex = OS_MUTEX_STATIC_INITIALISER;
static OS_semaphore_t semaphore = OS_SEMAPHORE_STATIC_INITIALISER;

__attribute__((noreturn))
static void task1(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("AAAAAAAA\r\n");
		OS_mutex_release(&mutex);

	}
}

__attribute__((noreturn))
static void task2(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(5);
		OS_mutex_acquire(&mutex);
		printf("BBBBBBBB\r\n");
		OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task3(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(50);
		//OS_mutex_acquire(&mutex);
		OS_semaphore_acquire(&semaphore);
		printf("CCCCCCCC\r\n");
		OS_semaphore_release(&semaphore);
		//OS_mutex_release(&mutex);
	}
}

__attribute__((noreturn))
static void task4(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(1000);
		OS_semaphore_acquire(&semaphore);
		printf("DDDDDDDD\r\n");
		OS_semaphore_release(&semaphore);
	}
}

__attribute__((noreturn))
static void task5(void const *const args) {
	(void) args;
	while (1) {
		//OS_sleep(2000);
		OS_semaphore_acquire(&semaphore);
		printf("EEEEEEEE\r\n");
		OS_semaphore_release(&semaphore);
	}
}

__attribute__((noreturn))
static void task6(void const *const args) {
	(void) args;
	while (1) {
		
//		void *block1 = pool_allocate_from(0);
//		if (block1) {
//			printf("First block allocated at address %p\n", block1);
//			//pool_deallocate_to(0, block1); // Deallocate back to pool 0
//		}

//		// Allocate another block from the same pool
//		void *block2 = pool_allocate_from(0);
//		if (block2) {
//			printf("Second block allocated at address %p\n", block2);
//			// Optional: Deallocation of block2
//			//pool_deallocate_to(0, block2);
//		}
		
		void *block3 = pool_allocate_from(2);
		if (block3) {
			printf("Third block allocated at address %p\n", block3);
			//pool_deallocate_to(1, block3); // Deallocate back to pool 0
		}
		
		void *block4 = pool_allocate_from(2);
		if (block4) {
			printf("Fourth block allocated at address %p\n", block4);
			//pool_deallocate_to(1, block4); // Deallocate back to pool 0
		}
		
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
	static uint32_t stack6[128] __attribute__ (( aligned(8) ));
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5, TCB6;
	
	/* Initialise the TCBs using the two functions above */
	
	OS_initialiseTCB(&TCB1, stack1+128, task1, NULL, 2);
	OS_initialiseTCB(&TCB2, stack2+128, task2, NULL, 2);
	OS_initialiseTCB(&TCB3, stack3+128, task3, NULL, 2);
	OS_initialiseTCB(&TCB4, stack4+128, task4, NULL, 2);
	OS_initialiseTCB(&TCB5, stack5+128, task5, NULL, 2);
	OS_initialiseTCB(&TCB6, stack6+128, task6, NULL, 2);
	
	mempools_init();
	
	/* Add the tasks to the scheduler */
	
	//OS_addTask(&TCB1);
	//OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	OS_addTask(&TCB5);
	//OS_addTask(&TCB6);
	
	/* Start the OS */
	
	OS_start();
	
}
