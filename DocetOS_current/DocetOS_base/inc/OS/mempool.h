#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <stdint.h>
#include <stddef.h>

// 16KB pool size
#define STATIC_ALLOC_POOLSIZE 		16384UL
#define STATIC_ALLOC_ALIGNMENT 		8U
#define STATIC_ALLOC_MASK 				~(STATIC_ALLOC_ALIGNMENT - 1)

static uint8_t static_pool[STATIC_ALLOC_POOLSIZE] __attribute__ (( aligned(STATIC_ALLOC_ALIGNMENT) ));
static size_t pool_index = STATIC_ALLOC_POOLSIZE;

typedef struct mempool_item_s {
	struct mempool_item_s *next;
} mempool_item_t;

typedef struct {
	mempool_item_t *head;
} mempool_t;

void *static_alloc(size_t bytes);
void *pool_allocate(mempool_t *pool);
void pool_deallocate(mempool_t *pool, void *block);
void pool_init(mempool_t *pool, size_t blocksize, size_t blocks);
void mempools_init(void);
void *pool_allocate_from(int pool_num);
void pool_deallocate_to(int pool_num, void *block);

#endif /* MEMPOOL_H */
