#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("char: %d\r\n", sizeof(char));

	while(1);
}
