#include "Utils/utils.h"
#include <stdio.h>

typedef struct {
	int8_t x;
	int32_t y;
} test_t;

int main(void) {
	
	configClock();
	configUSART2(38400);
		
	test_t test;

	while(1);
}
