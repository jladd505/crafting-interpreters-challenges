#ifndef CHAPTER21_GLOBALS_H
#define CHAPTER21_GLOBALS_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_GLOBALS 256
#define NAME_BUCKETS 512

typedef struct {
  char* name;
  unsigned char index;
  bool occupied;
} NameEntry;

typedef struct {
  NameEntry entries[NAME_BUCKETS];
  const char* names[MAX_GLOBALS];
  size_t count;
} NamePool;

typedef struct {
  NamePool pool;
  double values[MAX_GLOBALS];
  bool defined[MAX_GLOBALS];
} GlobalStore;

void initNamePool(NamePool* pool);
void freeNamePool(NamePool* pool);
bool internName(NamePool* pool, const char* name, unsigned char* index);

void initGlobalStore(GlobalStore* store);
void freeGlobalStore(GlobalStore* store);
bool globalSlot(GlobalStore* store, const char* name, unsigned char* slot);
void defineGlobal(GlobalStore* store, unsigned char slot, double value);
bool getGlobal(const GlobalStore* store, unsigned char slot, double* value);
bool setGlobal(GlobalStore* store, unsigned char slot, double value);

#endif

