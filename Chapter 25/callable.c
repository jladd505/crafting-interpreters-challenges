#include "callable.h"

#include <stdlib.h>
#include <string.h>

static Closure* closure_create(const Function* function, const int* captures) {
  Closure* closure = malloc(sizeof(*closure));
  if (closure == NULL) return NULL;

  closure->function = function;
  closure->captures = NULL;
  if (function->upvalue_count == 0) return closure;

  closure->captures = malloc(function->upvalue_count * sizeof(int));
  if (closure->captures == NULL) {
    free(closure);
    return NULL;
  }
  memcpy(closure->captures, captures,
         function->upvalue_count * sizeof(int));
  return closure;
}

Callable callable_create_selective(const Function* function,
                                   const int* captures) {
  Callable callable;
  if (function->upvalue_count == 0) {
    callable.kind = CALLABLE_FUNCTION;
    callable.as.function = function;
  } else {
    callable.kind = CALLABLE_CLOSURE;
    callable.as.closure = closure_create(function, captures);
  }
  return callable;
}

Callable callable_create_always_wrapped(const Function* function,
                                        const int* captures) {
  Callable callable;
  callable.kind = CALLABLE_CLOSURE;
  callable.as.closure = closure_create(function, captures);
  return callable;
}

const Function* callable_function(const Callable* callable) {
  if (callable->kind == CALLABLE_FUNCTION) return callable->as.function;
  if (callable->as.closure == NULL) return NULL;
  return callable->as.closure->function;
}

bool callable_call(const Callable* callable, int arg, int* result) {
  const Function* function = callable_function(callable);
  if (function == NULL || function->body == NULL || function->arity != 1) {
    return false;
  }

  const int* captures = NULL;
  size_t capture_count = 0;
  if (callable->kind == CALLABLE_CLOSURE) {
    captures = callable->as.closure->captures;
    capture_count = function->upvalue_count;
  }

  *result = function->body(captures, capture_count, arg);
  return true;
}

void callable_destroy(Callable* callable) {
  if (callable->kind == CALLABLE_CLOSURE && callable->as.closure != NULL) {
    free(callable->as.closure->captures);
    free(callable->as.closure);
    callable->as.closure = NULL;
  }
}

