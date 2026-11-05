#include "globals.h"

#include <stdlib.h>
#include <string.h>

static unsigned hashName(const char* name) {
  unsigned hash = 2166136261u;
  while (*name) { hash ^= (unsigned char)*name++; hash *= 16777619u; }
  return hash;
}

void initNamePool(NamePool* pool) {
  memset(pool, 0, sizeof(*pool));
}

void freeNamePool(NamePool* pool) {
  for (size_t i = 0; i < NAME_BUCKETS; i++) {
    if (pool->entries[i].occupied) free(pool->entries[i].name);
  }
  initNamePool(pool);
}

bool internName(NamePool* pool, const char* name, unsigned char* index) {
  unsigned bucket = hashName(name) & (NAME_BUCKETS - 1);
  for (;;) {
    NameEntry* entry = &pool->entries[bucket];
    if (!entry->occupied) {
      if (pool->count >= MAX_GLOBALS) return false;
      size_t length = strlen(name);
      entry->name = malloc(length + 1);
      if (entry->name == NULL) return false;
      memcpy(entry->name, name, length + 1);
      entry->index = (unsigned char)pool->count;
      entry->occupied = true;
      pool->names[pool->count++] = entry->name;
      *index = entry->index;
      return true;
    }
    if (strcmp(entry->name, name) == 0) {
      *index = entry->index;
      return true;
    }
    bucket = (bucket + 1) & (NAME_BUCKETS - 1);
  }
}

void initGlobalStore(GlobalStore* store) {
  initNamePool(&store->pool);
  memset(store->values, 0, sizeof(store->values));
  memset(store->defined, 0, sizeof(store->defined));
}

void freeGlobalStore(GlobalStore* store) { freeNamePool(&store->pool); }

bool globalSlot(GlobalStore* store, const char* name, unsigned char* slot) {
  return internName(&store->pool, name, slot);
}

void defineGlobal(GlobalStore* store, unsigned char slot, double value) {
  store->values[slot] = value;
  store->defined[slot] = true;
}

bool getGlobal(const GlobalStore* store, unsigned char slot, double* value) {
  if (!store->defined[slot]) return false;
  *value = store->values[slot];
  return true;
}

bool setGlobal(GlobalStore* store, unsigned char slot, double value) {
  if (!store->defined[slot]) return false;
  store->values[slot] = value;
  return true;
}

