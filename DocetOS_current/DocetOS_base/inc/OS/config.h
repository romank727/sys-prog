#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define MAX_TASK_PRIORITY_LEVELS	5
#define NUM_MEMORY_POOLS	3

typedef struct {
	size_t block_size;
	size_t num_blocks;
} mempool_config_t;

// Define the memory pool configurations
static const mempool_config_t mempool_config[NUM_MEMORY_POOLS] = {
    {16, 20},  // Pool 0: 16 bytes, 20 blocks
    {32, 15},  // Pool 1: 32 bytes, 15 blocks
    {64, 10}   // Pool 2: 64 bytes, 10 blocks
};

#endif /* CONFIG_H */
