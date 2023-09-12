#include "Utils/utils.h"
#include <stdio.h>

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	int i = 3;
	
	while(1);

}
