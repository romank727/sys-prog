#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

#pragma pack(push)
#pragma pack(1)

typedef struct {
	int32_t y;
	int32_t x;
	void (*callback) (void *);
	void *ptr;
} structure_t;

#pragma pack(pop)

void process(structure_t * s);

static void printInteger(void * x) {
	int32_t *y = x;
	printf("%" PRId32 "\r\n", *y);
}

static void printDouble(void * x) {
	double *y = x;
	printf("%f\r\n", *y);
}

int main(void) {
	configClock();
	configUSART2(38400);

	structure_t myStruct;
	structure_t myStruct2;
	myStruct.x = 8;
	myStruct.y = 5;
	myStruct2.x = 15;
	myStruct2.y = 20;
	
	int32_t var = 12;
	myStruct.ptr = &var;
	double var2 = 7;
	myStruct2.ptr = &var2;
	
	myStruct.callback = &printInteger;
	myStruct2.callback = &printDouble;
	
	process(&myStruct);
	process(&myStruct2);
	
	printf("struct 1 x: %" PRId32 "\r\n", myStruct.x);
	printf("struct 1 y: %" PRId32 "\r\n", myStruct.y);
	printf("struct 2 x: %" PRId32 "\r\n", myStruct2.x);
	printf("struct 2 y: %" PRId32 "\r\n", myStruct2.y);
	
	while(1);
}
