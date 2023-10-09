#include "Utils/utils.h"
#include <stdio.h>
#include <stdint.h>
#include "stack.h"

int main(void) {
	
	configClock();
	configUSART2(38400);
		
	// Array to hold stack entries
	int32_t stack[10];
	// Stack pointer, initially points to the first array element
	int32_t *stack_p = stack;

	push(&stack_p, 5);
  printf("Popped value: %d\r\n", pop(&stack_p));
	
}
