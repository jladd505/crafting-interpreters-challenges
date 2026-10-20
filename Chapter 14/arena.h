#ifndef CHAPTER14_ARENA_H
#define CHAPTER14_ARENA_H

#include <stddef.h>

#define ARENA_SIZE 65536u

void arenaInit(void);
void* arenaAllocate(size_t size);
void arenaFree(void* pointer);
void* arenaReallocate(void* pointer, size_t newSize);
size_t arenaFreeBytes(void);
size_t arenaLargestFreeBlock(void);

#endif

