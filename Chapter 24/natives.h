#ifndef CHAPTER24_NATIVES_H
#define CHAPTER24_NATIVES_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { VALUE_NIL, VALUE_BOOL, VALUE_NUMBER, VALUE_STRING } ValueType;
typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    const char* string;
  } as;
} Value;

#define NIL_VALUE ((Value){VALUE_NIL, {.number = 0}})
#define BOOL_VALUE(v) ((Value){VALUE_BOOL, {.boolean = (v)}})
#define NUMBER_VALUE(v) ((Value){VALUE_NUMBER, {.number = (v)}})
#define STRING_VALUE(v) ((Value){VALUE_STRING, {.string = (v)}})

typedef bool (*NativeFn)(int argCount, Value* args);

typedef struct {
  const char* name;
  int arity;
  NativeFn function;
} Native;

const Native* findNative(const char* name);
bool callNative(const Native* native, int argCount, Value* callSlots,
                char* error, size_t errorCapacity);
const char* valueTypeName(Value value);

#endif

