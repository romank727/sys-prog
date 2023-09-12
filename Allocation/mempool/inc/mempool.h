#ifndef MEMPOOL_H
#define MEMPOOL_H

struct mempool_item_s {
	struct mempool_item_s *next;
};
typedef struct mempool_item_s mempool_item_t;

typedef struct {
	mempool_item_t *head;
} mempool_t;

#define MEMPOOL_INITIALISER { .head = 0 }

void *pool_allocate(mempool_t *pool);
void pool_deallocate(mempool_t *pool, void *block);

#define pool_add pool_deallocate

#endif /* MEMPOOL_H */
