#ifndef CHAPTER25_CALLABLE_H
#define CHAPTER25_CALLABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*FunctionBody)(const int* captures, size_t capture_count, int arg);

typedef struct {
  int arity;
  size_t upvalue_count;
  FunctionBody body;
} Function;

typedef struct {
  const Function* function;
  int* captures;
} Closure;

typedef enum {
  CALLABLE_FUNCTION,
  CALLABLE_CLOSURE
} CallableKind;

typedef struct {
  CallableKind kind;
  union {
    const Function* function;
    Closure* closure;
  } as;
} Callable;

Callable callable_create_selective(const Function* function,
                                   const int* captures);
Callable callable_create_always_wrapped(const Function* function,
                                        const int* captures);
const Function* callable_function(const Callable* callable);
bool callable_call(const Callable* callable, int arg, int* result);
void callable_destroy(Callable* callable);

#endif

