#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <stdint.h>
#include <stddef.h>

/**
*	Define 16KB static memory pool size
*/
#define STATIC_ALLOC_POOLSIZE			16384UL

/**	
*	Define the alignment requirement for the memory pool as 8 bytes.
*	Makes sure allocated memory addresses are aligned to 8-byte boundaries only. 
*/
#define STATIC_ALLOC_ALIGNMENT		8U

/**	
*	Create a mask for ensuring alignment. The mask is used in 'static_alloc' function
*	to adjust the memory pool index to the nearest lower alignment boundary.
*	e.g. STATIC_ALLOC_ALIGNMENT is 8, the mask is 0xFFFFFFF8 
*/
#define STATIC_ALLOC_MASK					~(STATIC_ALLOC_ALIGNMENT - 1)

/**	
*	Declaring a static memory pool array which acts as a simple pool
*	from which memory blocks are allocated. 
*	The '__attribute__((aligned))' directive makes sure that the start 
*	of the pool is aligned to the specified boundary (8 bytes). 
*/
static uint8_t static_pool[STATIC_ALLOC_POOLSIZE] __attribute__ (( aligned(STATIC_ALLOC_ALIGNMENT) ));

/**	
*	Initialize the pool index to the size of the pool. 'pool_index' keeps track
*	of the current position in the pool for the next allocation. It starts at the
*	end of the pool and moves backward as memory is allocated. 
*/
static size_t pool_index = STATIC_ALLOC_POOLSIZE;

/**	
*	Structure representing an item in a memory pool. Used to link
*	free blocks in the pool, allowing for efficient use.
*/
typedef struct mempool_item_s {
	//	Pointer to next item in memory pool
	//	This forms a singly linked list of free memory blocks
	struct mempool_item_s *next;
} mempool_item_t;

/**
*	Structure representing a memory pool. This manages a collection of
*	memory blocks that can be allocated/deallocated.
*/
typedef struct {
	//	Pointer to the first item in the pool.
	//	When pool is empty, this is NULL
	//	When a block is allocated, this pointer moves to the next free block.
	mempool_item_t *head;
} mempool_t;

void *static_alloc(size_t bytes);
void *pool_allocate(mempool_t *pool);
void pool_deallocate(mempool_t *pool, void *block);
void pool_init(mempool_t *pool, size_t blocksize, size_t blocks);
void mempools_init(void);
void *pool_allocate_from(int32_t pool_num);
void pool_deallocate_to(int32_t pool_num, void *block);

#endif /* MEMPOOL_H */
