#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

#pragma pack(push)
#pragma pack(1)

typedef struct {
	int32_t x;
	int32_t y;
	void *ptr;
	void (*callback) (void *);
} structure_t;

#pragma pack(pop)

void process(structure_t * s);

static void printInteger(void * x) {
	int32_t *y = x;
	printf("%" PRId32 "\r\n", *y);
}

int main(void) {
	configClock();
	configUSART2(38400);

	structure_t myStruct;
	myStruct.x = 8;
	myStruct.y = 5;
	
	int32_t var = 12;
	myStruct.ptr = &var;
	
	myStruct.callback = &printInteger;
	
	process(&myStruct);
	
	printf("%" PRId32 "\r\n", myStruct.x);
	printf("%" PRId32 "\r\n", myStruct.y);
	
	while(1);
}
