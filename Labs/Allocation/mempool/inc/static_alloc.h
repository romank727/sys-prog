#ifndef STATIC_ALLOC_H
#define STATIC_ALLOC_H
#include <stddef.h>

#define STATIC_ALLOC_ALIGNMENT 8U
#define STATIC_ALLOC_MASK ~(STATIC_ALLOC_ALIGNMENT - 1)

void * static_alloc(size_t bytes);

#endif /* STATIC_ALLOC_H */
