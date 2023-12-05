#include "Utils/utils.h"
#include <stdio.h>

static int processArray(int *array) {
	return *(array + 2) = 10;
}

int main(void) {
	
	configClock();
	configUSART2(38400);

	int numbers[] = {1,3,5,7,9};
	printf("System start\r\n");
	
	//numbers[0] = 11;
	*numbers = 11;
	//numbers[3] = 42;
	*(numbers + 3) = 42;
	
	for (int i = 0; i < 5; ++i) {
		printf("numbers[%d] = %d\n", i, numbers[i]);
	}
	
	int *pointer = numbers;
	for (int i = 0; i < 5; ++i) {
		printf("pointer[%d] = %d\r\n", i, pointer[i]);
	}
	
	printf("numbers: %d\r\n", sizeof(numbers));
	printf("pointer: %d\r\n", sizeof(pointer));
	
	printf("3rd value before: %d\r\n", numbers[2]);
	*(numbers + 2) = processArray(numbers);
	printf("3rd value after: %d\r\n", numbers[2]);
	
	while(1);

}
