#include "OS/mempool.h"
#include "OS/config.h"
#include "stm32f4xx.h"

static mempool_t mempools[NUM_MEMORY_POOLS];

void *pool_allocate(mempool_t *pool) {
	mempool_item_t *head;
	do {
		head = (mempool_item_t *)__LDREXW((volatile uint32_t *)&pool->head);
		if (!head) {
			// List is empty, nothing to allocate
			__CLREX(); // Clear exclusive access
			return 0;
		}
		// Attempt to update the head of the pool atomically
	} while (__STREXW((uint32_t)head->next, (volatile uint32_t *)&pool->head));
	
	return head;
}

void pool_deallocate(mempool_t *pool, void *block) {
	mempool_item_t *item = block;
	mempool_item_t *oldHead;
	do {
		// Load the current head and set exclusive access
		oldHead = (mempool_item_t *)__LDREXW((volatile uint32_t *)&pool->head);
		// Point new block to the old head
		item->next = oldHead;
		// Attempt to update the head of the pool atomically
	} while (__STREXW((uint32_t)item, (volatile uint32_t *)&pool->head));
}

void pool_init(mempool_t *pool, size_t blocksize, size_t blocks) {
	blocksize = (blocksize + (STATIC_ALLOC_ALIGNMENT - 1)) & STATIC_ALLOC_MASK;
	void *memoryBlock = static_alloc(blocks * blocksize);
	if (!memoryBlock) {
		pool->head = 0;
		return;
	}
	for (size_t i = 0; i < blocks; ++i) {
		pool_deallocate(pool, (uint8_t *)memoryBlock + i * blocksize);
	}
}

void mempools_init(void) {
	for (uint32_t i = 0; i < NUM_MEMORY_POOLS; i++) {
		pool_init(&mempools[i], mempool_config[i].block_size, mempool_config[i].num_blocks);
	}
}

void *pool_allocate_from(int32_t pool_num) {
	if (pool_num >= 0 && pool_num < NUM_MEMORY_POOLS) {
		return pool_allocate(&mempools[pool_num]);
	}
	return 0;
}

void pool_deallocate_to(int32_t pool_num, void *block) {
	if (pool_num >= 0 && pool_num < NUM_MEMORY_POOLS) {
		pool_deallocate(&mempools[pool_num], block);
	}
}

void * static_alloc(size_t bytes) {
	if (!bytes || bytes > pool_index) {
		return 0;
	}
	pool_index -= bytes;
	pool_index &= STATIC_ALLOC_MASK;
	
	return static_pool + pool_index;
}
