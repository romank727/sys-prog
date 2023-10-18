#include "mempool.h"

void *pool_allocate(mempool_t *pool) {
	// Return the head of the list of blocks
	// Update the head pointer
	if(!pool->head) {
		return 0;
	}
	mempool_item_t * head = pool->head;
	pool->head = head->next;
	return head;
}

void pool_deallocate(mempool_t *pool, void *block) {
	// Add the new item to the head of the list
	/*
		Store the previous head before adding the new block.
		When added into the pool, point to the old head.
		Update the new head.
	*/
	/*
	same code as below: 
	mempool_item_t * prevHead = pool->head;
	((mempool_item_t*)block)->next = prevHead;
	pool->head = block;
	*/
	mempool_item_t * item = block;
	item->next = pool->head;
	pool->head = item;
}

void pool_init(mempool_t *pool, size_t blocksize, size_t blocks) {
	// Rounding up.
	uint32_t remainder = blocksize % STATIC_ALLOC_ALIGNMENT;
	if (remainder != 0) {
		// Finding how many bytes are missing and adding them to align.
		blocksize += STATIC_ALLOC_ALIGNMENT - remainder;
	}
	// Total memory needed
	size_t totalMem = blocks * blocksize;
	// Statically allocate total memory
	void * totalMemPtr = static_alloc(totalMem);
	if (totalMemPtr == 0) {
		pool->head = 0;
	}
	uint32_t * addressPtr = (uint32_t *) totalMemPtr;
	// A loop for each block with its own start address
	for (size_t i = 0; i < blocks; ++i) {
		void * address = addressPtr + (i * blocksize);
		pool_add(pool, address);
	}
}
