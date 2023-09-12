#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	char array[] = "Hello!";
	char * literal = "Hello!";

	printf("array: %s\n", array);
	printf("literal: %s\n", literal);

	printf("array is stored at %p\n", (void *)array);
	printf("literal is stored at %p\n", (void *)literal);

	while(1);
}
