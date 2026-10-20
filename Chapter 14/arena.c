#include "arena.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct Block {
  size_t size;
  bool free;
  struct Block* previous;
  struct Block* next;
} Block;

typedef union {
  max_align_t alignment;
  unsigned char bytes[ARENA_SIZE];
} AlignedHeap;

static AlignedHeap heap;
static Block* first;

static size_t alignSize(size_t size) {
  size_t alignment = _Alignof(max_align_t);
  return (size + alignment - 1) & ~(alignment - 1);
}

static void mergeNext(Block* block) {
  Block* next = block->next;
  if (next == NULL || !next->free) return;
  block->size += sizeof(Block) + next->size;
  block->next = next->next;
  if (block->next != NULL) block->next->previous = block;
}

static void split(Block* block, size_t wanted) {
  if (block->size < wanted + sizeof(Block) + _Alignof(max_align_t)) return;
  unsigned char* address = (unsigned char*)(block + 1) + wanted;
  Block* remainder = (Block*)address;
  remainder->size = block->size - wanted - sizeof(Block);
  remainder->free = true;
  remainder->previous = block;
  remainder->next = block->next;
  if (remainder->next != NULL) remainder->next->previous = remainder;
  block->next = remainder;
  block->size = wanted;
}

void arenaInit(void) {
  first = (Block*)heap.bytes;
  first->size = ARENA_SIZE - sizeof(Block);
  first->free = true;
  first->previous = NULL;
  first->next = NULL;
}

void* arenaAllocate(size_t size) {
  if (size == 0) return NULL;
  size = alignSize(size);
  for (Block* block = first; block != NULL; block = block->next) {
    if (!block->free || block->size < size) continue;
    split(block, size);
    block->free = false;
    return block + 1;
  }
  return NULL;
}

void arenaFree(void* pointer) {
  if (pointer == NULL) return;
  Block* block = (Block*)pointer - 1;
  block->free = true;
  mergeNext(block);
  if (block->previous != NULL && block->previous->free) mergeNext(block->previous);
}

void* arenaReallocate(void* pointer, size_t newSize) {
  if (pointer == NULL) return arenaAllocate(newSize);
  if (newSize == 0) {
    arenaFree(pointer);
    return NULL;
  }

  Block* block = (Block*)pointer - 1;
  size_t wanted = alignSize(newSize);
  if (wanted <= block->size) {
    split(block, wanted);
    return pointer;
  }
  if (block->next != NULL && block->next->free &&
      block->size + sizeof(Block) + block->next->size >= wanted) {
    mergeNext(block);
    split(block, wanted);
    block->free = false;
    return pointer;
  }

  void* replacement = arenaAllocate(newSize);
  if (replacement == NULL) return NULL;
  memcpy(replacement, pointer, block->size < newSize ? block->size : newSize);
  arenaFree(pointer);
  return replacement;
}

size_t arenaFreeBytes(void) {
  size_t total = 0;
  for (Block* block = first; block != NULL; block = block->next) {
    if (block->free) total += block->size;
  }
  return total;
}

size_t arenaLargestFreeBlock(void) {
  size_t largest = 0;
  for (Block* block = first; block != NULL; block = block->next) {
    if (block->free && block->size > largest) largest = block->size;
  }
  return largest;
}

