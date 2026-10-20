#include <stdio.h>
#include <time.h>

#define ITERATIONS 50000000

static double seconds(clock_t start, clock_t end) {
  return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(void) {
  volatile double stack[2] = {1.25, 0.0};
  volatile int top = 1;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    double value = stack[--top];
    stack[top++] = -value;
  }
  clock_t middle = clock();
  for (int i = 0; i < ITERATIONS; i++) stack[top - 1] = -stack[top - 1];
  clock_t end = clock();
  printf("pop/push negate: %.6f seconds\n", seconds(start, middle));
  printf("in-place negate: %.6f seconds\n", seconds(middle, end));
  printf("ignore: %.2f\n", stack[0]);
  return 0;
}
