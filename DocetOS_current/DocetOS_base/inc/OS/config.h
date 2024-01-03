#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

/**
*	This allows the user to configure the number of distinct priority levels for task scheduling.
*	The calculations with this priority number is assumed to be zero-indexed.
*	In other words, if you choose X, the highest available priority will be 'X-1'.
*	e.g.	X = 5;
*				highest priority given to a task = 4;
*/
#define MAX_TASK_PRIORITY_LEVELS	5

/**
*	This enables the user to define the quantity of memory pools they want.
*	This value is also assumed to be zero-indexed in further calculations.
*	e.g.	The first memory pool is pool '0'
*				The second memory pool is pool '1' etc.
*/
#define NUM_MEMORY_POOLS	3

/**
*	A structure to define the configuration of a memory pool.
*	block_size: The size of each block in bytes.
*	num_blocks: The total number of blocks in this memory pool
*/
typedef struct {
	size_t block_size;
	size_t num_blocks;
} mempool_config_t;

/** 
*	mempool_config: Array defining the configurations for each memory pool.
*									Intended to allow the user to specify their own 
*									block amount and the size of those blocks.
*
*	Each entry in the array corresponds to a different memory pool, specifying
*	its block size and the total number of blocks.
*/
static const mempool_config_t mempool_config[NUM_MEMORY_POOLS] = {
    {16, 20},  // Pool 0: 16 bytes, 20 blocks
    {32, 15},  // Pool 1: 32 bytes, 15 blocks
    {64, 10}   // Pool 2: 64 bytes, 10 blocks
};

#endif /* CONFIG_H */
