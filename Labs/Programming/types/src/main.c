#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("System start: \n\n");
	printf("char: %d\r\n", sizeof(char));
	printf("int: %d\r\n", sizeof(int));
	printf("float: %d\r\n", sizeof(float));
	printf("double: %d\r\n\n", sizeof(double));
	
	int x = 42000U;
	int y = 42;
	y = x;
	printf("x: %d\r\n", x);
	printf("y: %d\r\n", y);
	
	unsigned int mask = (0xBAUL) << 24;
	printf("mask: 0x%08X\r\n", mask);

	while(1);
}
