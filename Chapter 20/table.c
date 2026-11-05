#include "table.h"

#include <stdlib.h>
#include <string.h>

#define MAX_LOAD 0.75

static uint32_t mix64(uint64_t bits) {
  bits ^= bits >> 33;
  bits *= UINT64_C(0xff51afd7ed558ccd);
  bits ^= bits >> 33;
  bits *= UINT64_C(0xc4ceb9fe1a85ec53);
  bits ^= bits >> 33;
  return (uint32_t)(bits ^ (bits >> 32));
}

uint32_t hashValue(Value value) {
  switch (value.type) {
    case VAL_NIL: return 0x9e3779b9u;
    case VAL_BOOL: return value.as.boolean ? 0x85ebca6bu : 0xc2b2ae35u;
    case VAL_NUMBER: {
      double canonical = value.as.number == 0.0 ? 0.0 : value.as.number;
      uint64_t bits;
      memcpy(&bits, &canonical, sizeof(bits));
      return mix64(bits);
    }
    case VAL_STRING: {
      uint32_t hash = 2166136261u;
      for (const unsigned char* p = (const unsigned char*)value.as.string; *p; p++) {
        hash ^= *p;
        hash *= 16777619u;
      }
      return hash;
    }
    case VAL_EMPTY: case VAL_TOMBSTONE: return 0;
  }
  return 0;
}

bool valuesEqual(Value left, Value right) {
  if (left.type != right.type) return false;
  switch (left.type) {
    case VAL_NIL: case VAL_EMPTY: case VAL_TOMBSTONE: return true;
    case VAL_BOOL: return left.as.boolean == right.as.boolean;
    case VAL_NUMBER: return left.as.number == right.as.number;
    case VAL_STRING: return strcmp(left.as.string, right.as.string) == 0;
  }
  return false;
}

void initTable(Table* table, bool forceCollisions) {
  table->count = 0;
  table->tombstones = 0;
  table->capacity = 0;
  table->entries = NULL;
  table->forceCollisions = forceCollisions;
}

void freeTable(Table* table) {
  free(table->entries);
  initTable(table, table->forceCollisions);
}

static uint32_t tableHash(const Table* table, Value key) {
  return table->forceCollisions ? 1u : hashValue(key);
}

static Entry* findEntry(const Table* table, Entry* entries, size_t capacity, Value key) {
  size_t index = tableHash(table, key) & (capacity - 1);
  Entry* tombstone = NULL;
  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key.type == VAL_EMPTY) return tombstone != NULL ? tombstone : entry;
    if (entry->key.type == VAL_TOMBSTONE) {
      if (tombstone == NULL) tombstone = entry;
    } else if (valuesEqual(entry->key, key)) {
      return entry;
    }
    index = (index + 1) & (capacity - 1);
  }
}

static bool resize(Table* table, size_t capacity) {
  Entry* entries = malloc(sizeof(Entry) * capacity);
  if (entries == NULL) return false;
  for (size_t i = 0; i < capacity; i++) {
    entries[i].key = EMPTY_VAL;
    entries[i].value = NIL_VAL;
  }
  size_t oldCapacity = table->capacity;
  Entry* oldEntries = table->entries;
  table->count = 0;
  table->tombstones = 0;
  table->capacity = capacity;
  table->entries = entries;
  for (size_t i = 0; i < oldCapacity; i++) {
    if (oldEntries[i].key.type == VAL_EMPTY || oldEntries[i].key.type == VAL_TOMBSTONE) continue;
    Entry* dest = findEntry(table, entries, capacity, oldEntries[i].key);
    dest->key = oldEntries[i].key;
    dest->value = oldEntries[i].value;
    table->count++;
  }
  free(oldEntries);
  return true;
}

bool tableSet(Table* table, Value key, Value value) {
  if (key.type == VAL_EMPTY || key.type == VAL_TOMBSTONE) return false;
  if (table->capacity == 0 || table->count + table->tombstones + 1 > (size_t)(table->capacity * MAX_LOAD)) {
    size_t capacity = table->capacity < 8 ? 8 : table->capacity * 2;
    if (!resize(table, capacity)) return false;
  }
  Entry* entry = findEntry(table, table->entries, table->capacity, key);
  bool isNew = entry->key.type == VAL_EMPTY || entry->key.type == VAL_TOMBSTONE;
  if (entry->key.type == VAL_TOMBSTONE) table->tombstones--;
  if (isNew) table->count++;
  entry->key = key;
  entry->value = value;
  return isNew;
}

bool tableGet(const Table* table, Value key, Value* value) {
  if (table->count == 0) return false;
  Entry* entry = findEntry(table, table->entries, table->capacity, key);
  if (entry->key.type == VAL_EMPTY || entry->key.type == VAL_TOMBSTONE) return false;
  *value = entry->value;
  return true;
}

bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;
  Entry* entry = findEntry(table, table->entries, table->capacity, key);
  if (entry->key.type == VAL_EMPTY || entry->key.type == VAL_TOMBSTONE) return false;
  entry->key = TOMBSTONE_VAL;
  entry->value = NIL_VAL;
  table->count--;
  table->tombstones++;
  return true;
}

