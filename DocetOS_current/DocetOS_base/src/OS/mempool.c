#include "OS/mempool.h"
#include "OS/config.h"
#include "stm32f4xx.h"

static mempool_t mempools[NUM_MEMORY_POOLS];

/**
*	Allocates a block of memory from the specified memory pool.
*	This function ensures thread-safe allocation from a memory pool in
*	a concurrent environment. It retrieves and removes the head block
*	from the pool, making it available for use.
*
*	@param	pool	Pointer to 'mempool_t' struct representing the memory pool
*								from which the block is to be allocated.
*
*	@return				Pointer to the allocated block of memory if successful.
*								NULL if memory pool is empty or if allocation fails
*	
*	1.	Atomically load the current head of the memory pool. LDREX
*			is used to ensure safety.
*	2.	If the pool's head is NULL (pool is empty), clear the exclusive flag
*			and return NULL
*	3.	If the pool isn't empty, try to atomically update the head of the pool
*			using STREX. This condition is in a loop to make sure the operation succeeds
*			eventually.
*	4.	Return the original head of the pool, which is now allocated for use.
*/
void *pool_allocate(mempool_t *pool) {
	mempool_item_t *head;
	do {
		head = (mempool_item_t *)__LDREXW((volatile uint32_t *)&pool->head);
		if (!head) {
			__CLREX(); // Clear exclusive access
			return 0;
		}
	} while (__STREXW((uint32_t)head->next, (volatile uint32_t *)&pool->head));
	
	return head;
}

/**
*	Deallocates (returns) a block of memory back to the specified memory pool.
*	This function tries to do so atomically to ensure safety in a multi-threaded
*	environment.
*	
*	@param	pool		Pointer to 'mempool_t' struct, representing the memory pool
*									to which the block is being deallocated.
*
*	@param	block		Pointer to the block of memory being deallocated
*	
*	1.	Treat 'block' as an item of the memory pool by casting it to
*			'mempool_item_t'. This allows the block to be linked back into the pool.
*	2.	Enter a do...while loop to try for an atomic deallocation:
*				-	Atomically load the current head of the pool using LDREX.
*					This also sets the exclusive access flag.
*				-	Set the 'next' pointer of 'item' to the current head, which basically
*					places it at the front of the pool.
*				-	Try to atomically update the head of the pool to the deallocated block
*					using STREX. This will succeed only if the exclusive access is still valid.
*				-	If exclusive access is lost, repeat the loop and try again until success.
*/
void pool_deallocate(mempool_t *pool, void *block) {
	mempool_item_t *item = block;
	mempool_item_t *oldHead;
	do {
		oldHead = (mempool_item_t *)__LDREXW((volatile uint32_t *)&pool->head);
		item->next = oldHead;
	} while (__STREXW((uint32_t)item, (volatile uint32_t *)&pool->head));
}

/**
*	Initialises a memory pool for allocation/deallocation of fixed-sized blocks
*	by dividing a large memory block into smaller blocks.
*	
*	@param	pool				Pointer to the memory pool to initialise.
*
*	@param	blocksize		The size of each block in the pool. This size will
*											be aligned to the defined alignment (STATIC_ALLOC_ALIGNMENT).
*
*	@param	blocks			The number of blocks to create in the memory pool.
*
*	The function adjusts blocksize to meet the alignment and then allocates
*	a memory block large enough to hold all the blocks.
*	It then iterates through the allocated memory, dividing it into individual
*	blocks and deallocating after.
*/
void pool_init(mempool_t *pool, size_t blocksize, size_t blocks) {
	// Align the block size
	blocksize = (blocksize + (STATIC_ALLOC_ALIGNMENT - 1)) & STATIC_ALLOC_MASK;
	// Allocate a memory block for all the blocks in the pool
	void *memoryBlock = static_alloc(blocks * blocksize);
	// If memory allocation failed, set pool's head to NULL and return
	if (!memoryBlock) {
		pool->head = 0;
		return;
	}
	// Iterate through the allocated memory
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

/**
*	Allocates a specified number of bytes from a pre-allocated 
*	static memory pool. The pool is defined with a fixed size and alignment.
*	
*	@param	bytes		The number of bytes to allocate
*	
*	@return					A pointer to the allocated memory block.
*									NULL if the allocation fails.
*/
void * static_alloc(size_t bytes) {
	// Check if the requested size is zero or exceeds the available pool space.
	// If it is, return NULL to indicate allocation failure.
	if (!bytes || bytes > pool_index) {
		return 0;
	}
	
	// Decrement the pool index by the number of bytes requested.
	// Moving the allocation pointer downwards in the memory pool.
	pool_index -= bytes;
	
	// Apply the alignment mask to ensure the allocated memory block is properly aligned.
	pool_index &= STATIC_ALLOC_MASK;
	
	// Return a pointer to the start of the allocated memory block.
	// This pointer is an offset from the base address of the static pool.
	return static_pool + pool_index;
}
