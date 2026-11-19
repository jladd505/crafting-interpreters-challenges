#include "callable.h"
#include "loop_capture.h"

#include <assert.h>
#include <stdio.h>

static int add_one(const int* captures, size_t capture_count, int arg) {
  (void)captures;
  (void)capture_count;
  return arg + 1;
}

static int add_capture(const int* captures, size_t capture_count, int arg) {
  assert(capture_count == 1);
  return captures[0] + arg;
}

static void test_selective_wrapping(void) {
  const Function plain = {1, 0, add_one};
  Callable direct = callable_create_selective(&plain, NULL);
  assert(direct.kind == CALLABLE_FUNCTION);
  int result = 0;
  assert(callable_call(&direct, 41, &result));
  assert(result == 42);
  callable_destroy(&direct);

  const Function capturing = {1, 1, add_capture};
  const int capture[] = {10};
  Callable closure = callable_create_selective(&capturing, capture);
  assert(closure.kind == CALLABLE_CLOSURE);
  assert(callable_call(&closure, 32, &result));
  assert(result == 42);
  callable_destroy(&closure);
}

static void test_fresh_loop_variables(void) {
  LoopClosure closures[3];
  assert(loop_closures_create_fresh(1, 3, closures));
  assert(loop_closure_get(&closures[0]) == 1);
  assert(loop_closure_get(&closures[1]) == 2);
  assert(loop_closure_get(&closures[2]) == 3);

  loop_closure_set(&closures[0], 99);
  assert(loop_closure_get(&closures[0]) == 99);
  assert(loop_closure_get(&closures[1]) == 2);

  for (size_t i = 0; i < 3; i++) loop_closure_destroy(&closures[i]);
}

int main(void) {
  test_selective_wrapping();
  test_fresh_loop_variables();
  puts("All Chapter 25 tests passed.");
  return 0;
}

