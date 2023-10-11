#include "Utils/utils.h"
#include <stdio.h>
#include <inttypes.h>
#include "heap.h"

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	int32_t store[20];
	heap_t heap = HEAP_INITIALISER(store);

	// Remember not to insert so many things that the heap overflows!
	heap_insert(&heap, 2);
	heap_insert(&heap, 41);
	heap_insert(&heap, 83);
	heap_insert(&heap, 93);
	heap_insert(&heap, 7);
	heap_insert(&heap, 34);
	heap_insert(&heap, 48);
	heap_insert(&heap, 17);
	heap_insert(&heap, 35);
	heap_insert(&heap, 29);
	heap_insert(&heap, 87);
	heap_insert(&heap, 36);
	heap_insert(&heap, 21);
	heap_insert(&heap, 10);
	heap_insert(&heap, 25);
	heap_insert(&heap, 36);
	heap_insert(&heap, 77);
	heap_insert(&heap, 65);
	heap_insert(&heap, 63);

	
	while (!heap_isEmpty(&heap)) {
		printf("Extracted %" PRId32 "\r\n", heap_extract(&heap));
	}

	while(1);
}
