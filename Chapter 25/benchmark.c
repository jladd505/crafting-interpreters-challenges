#include "callable.h"

#include <stdio.h>
#include <time.h>

#define ITERATIONS 2000000

static int identity(const int* captures, size_t capture_count, int arg) {
  (void)captures;
  (void)capture_count;
  return arg;
}

static double elapsed(clock_t start, clock_t end) {
  return (double)(end - start) / CLOCKS_PER_SEC;
}

static double run(int always_wrap, long long* checksum) {
  const Function function = {1, 0, identity};
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    Callable callable = always_wrap
        ? callable_create_always_wrapped(&function, NULL)
        : callable_create_selective(&function, NULL);
    int result = 0;
    if (!callable_call(&callable, i, &result)) return -1.0;
    *checksum += result;
    callable_destroy(&callable);
  }
  return elapsed(start, clock());
}

int main(void) {
  long long always_checksum = 0;
  long long selective_checksum = 0;
  double always = run(1, &always_checksum);
  double selective = run(0, &selective_checksum);

  printf("iterations: %d\n", ITERATIONS);
  printf("always wrapped: %.6f seconds\n", always);
  printf("selective wrapping: %.6f seconds\n", selective);
  printf("speedup: %.2fx\n", always / selective);
  printf("checksums: %lld %lld\n", always_checksum, selective_checksum);
  return always_checksum == selective_checksum ? 0 : 1;
}

