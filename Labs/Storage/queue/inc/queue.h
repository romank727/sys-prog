#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 10

typedef struct {
	int32_t data[QUEUE_SIZE];
	uint_fast16_t insert;
	uint_fast16_t remove;
} queue_t;

#define QUEUE_INITIALISER { .data = {0}, .insert = 0, .remove = 0 }

void queue_put(queue_t * queue, int32_t data);
int32_t queue_get(queue_t * queue);

#endif /* QUEUE_H */
