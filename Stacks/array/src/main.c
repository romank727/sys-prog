#include "Utils/utils.h"
#include <stdio.h>

static void test(void) {
	printf("Hello!\r\n");
}

int main(void) {
	configClock();
	configUSART2(38400);
	
	uint32_t array[10];
	
	uint32_t * ptr = array;
	
	for (uint32_t i = 0; i < 10; i++) {
		*(ptr+i) = i;
	}
	
	*(ptr+11) = (uint32_t)(&test);
	
}
