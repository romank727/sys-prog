#include "Utils/utils.h"
#include <stdio.h>

#pragma pack(push)
#pragma pack(1)
typedef struct {
	int8_t x;
	int32_t y;
} test_t;
#pragma pack(pop)

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	test_t test = { .x = 100,
									.y = 5 };											
	printf("x: %d\r\n", test.x);
	printf("y: %d\r\n", test.y);
									
	test = (test_t) { .x = 50,
									.y = 10 };
	printf("x: %d\r\n", test.x);
	printf("y: %d\r\n", test.y);								
									
	test_t *test_p = &test;
	
	test_p->x = 5;
	test_p->y = 20;
	
	printf("x: %d\r\n", test.x);
	printf("y: %d\r\n", test.y);	
									
									
	while(1);
}
