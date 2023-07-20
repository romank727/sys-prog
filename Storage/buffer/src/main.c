#include "Utils/utils.h"
#include <stdio.h>

int queue[10];
int insert = 0;
int retrieve = 0;

void put(int item) {
	queue[insert] = item;
	insert = (insert + 1) % 10;
}

int get(void) {
	int value = queue[retrieve];
	retrieve = (retrieve + 1) % 10;
	return value;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
		
	put(4);
	put(7);
	put(2);
	printf("Removed %d from queue\r\n", get());
	printf("Removed %d from queue\r\n", get());
	printf("Removed %d from queue\r\n", get());
}
