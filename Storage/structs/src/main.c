#include "Utils/utils.h"
#include <stdio.h>

typedef struct {
	int x;
	int y;
} test_t;

int main(void) {
	
	configClock();
	configUSART2(38400);
		
	// Your code goes here
}
