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
	
		same code as: 
		mempool_item_t * prevHead = pool->head;
		((mempool_item_t*)block)->next = prevHead;
		pool->head = block;
	*/
	mempool_item_t * item = block;
	item->next = pool->head;
	pool->head = item;
}

void pool_init(mempool_t *pool, size_t blocksize, size_t blocks) {
	// Rounding up so it aligns with static_alloc
	blocksize = (blocksize + 7) & STATIC_ALLOC_MASK;
	// Total memory needed
	void * totalMemPtr = static_alloc(blocks * blocksize);
	// if totalMemPtr is a non-zero...
	if (totalMemPtr) {
			// A loop for each block with its own start address
			for (size_t i = 0; i < blocks; i++) {
				void * startAddress = ((uint8_t *)totalMemPtr) + (i * blocksize);
				pool_add(pool, startAddress);
			}
	}
	else {
		pool->head = 0;
	}
}
