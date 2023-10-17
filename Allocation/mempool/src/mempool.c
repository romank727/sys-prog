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
	
}
