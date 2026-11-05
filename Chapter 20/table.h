#ifndef CHAPTER20_TABLE_H
#define CHAPTER20_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { VAL_EMPTY, VAL_TOMBSTONE, VAL_NIL, VAL_BOOL, VAL_NUMBER, VAL_STRING } ValueType;
typedef struct {
  ValueType type;
  union { bool boolean; double number; const char* string; } as;
} Value;

#define EMPTY_VAL ((Value){VAL_EMPTY, {.number = 0}})
#define TOMBSTONE_VAL ((Value){VAL_TOMBSTONE, {.number = 0}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define BOOL_VAL(v) ((Value){VAL_BOOL, {.boolean = (v)}})
#define NUMBER_VAL(v) ((Value){VAL_NUMBER, {.number = (v)}})
#define STRING_VAL(v) ((Value){VAL_STRING, {.string = (v)}})

typedef struct { Value key; Value value; } Entry;
typedef struct {
  size_t count;
  size_t tombstones;
  size_t capacity;
  Entry* entries;
  bool forceCollisions;
} Table;

void initTable(Table* table, bool forceCollisions);
void freeTable(Table* table);
bool tableSet(Table* table, Value key, Value value);
bool tableGet(const Table* table, Value key, Value* value);
bool tableDelete(Table* table, Value key);
bool valuesEqual(Value left, Value right);
uint32_t hashValue(Value value);

#endif

