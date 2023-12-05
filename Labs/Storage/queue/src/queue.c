#include "queue.h"

void queue_put(queue_t * queue, int32_t data) {
	queue->data[queue->insert] = data;
	queue->insert = (queue->insert + 1) % QUEUE_SIZE;
}

int32_t queue_get(queue_t * queue) {
	int32_t data = queue->data[queue->remove];
	queue->remove = (queue->remove + 1) % QUEUE_SIZE;
	return data;
}
