#include "Utils/utils.h"
#include "queue.h"
#include <inttypes.h>
#include <stdio.h>

queue_t queue = QUEUE_INITIALISER;

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	queue_put(&queue, 4);
	queue_put(&queue, 7);
	queue_put(&queue, 2);
	printf("Removed %" PRId32 " from queue\r\n", queue_get(&queue));
	printf("Removed %" PRId32 " from queue\r\n", queue_get(&queue));
	printf("Removed %" PRId32 " from queue\r\n", queue_get(&queue));
}
