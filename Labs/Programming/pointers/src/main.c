#include "Utils/utils.h"
#include <stdio.h>

static void square(long long *i) {
	*i = *i * *i;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	printf("System start\n");
	long long i = 3;
	printf("address: %p\r\n", (void *)&i);
	printf("address: %p\r\n", (void *)(&i + 1));
	
	printf("i before: %lld\r\n", i);
	square(&i);
	printf("i after: %lld\r\n", i);
	
	while(1);

}
