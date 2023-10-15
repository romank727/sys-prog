#include "static_alloc.h"
#include <stdint.h>

// 16KB pool size
#define STATIC_ALLOC_POOLSIZE 16384UL

static uint8_t static_pool[STATIC_ALLOC_POOLSIZE];
static size_t pool_index = STATIC_ALLOC_POOLSIZE;

void * static_alloc(size_t bytes) {
	if (pool_index >= bytes) {
		pool_index -= bytes;
		return &static_pool[pool_index];
	}
	else {
		return 0;
	}
}
