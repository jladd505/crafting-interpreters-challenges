#include "ip_cache.h"

#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#define PROGRAM_SIZE 257
#define ROUNDS 200000

static double elapsed(clock_t start) {
  return (double)(clock() - start) / CLOCKS_PER_SEC;
}

int main(void) {
  uint8_t code[PROGRAM_SIZE];
  for (size_t i = 0; i < PROGRAM_SIZE - 1; i++) {
    code[i] = i % 5 == 0 ? BENCH_MIX : BENCH_INCREMENT;
  }
  code[PROGRAM_SIZE - 1] = BENCH_HALT;

  const uint8_t* finalIp;
  clock_t start = clock();
  uint64_t frameResult = runFrameIp(code, ROUNDS, &finalIp);
  double frameSeconds = elapsed(start);

  start = clock();
  uint64_t cachedResult = runCachedIp(code, ROUNDS, &finalIp);
  double cachedSeconds = elapsed(start);

  printf("Chapter 24 instruction-pointer benchmark\n");
  printf("frame field : %.6f s  checksum %" PRIu64 "\n", frameSeconds, frameResult);
  printf("cached local: %.6f s  checksum %" PRIu64 "\n", cachedSeconds, cachedResult);
  if (frameSeconds > 0) {
    printf("change      : %.2f%%\n", (cachedSeconds - frameSeconds) * 100.0 / frameSeconds);
  }
  return frameResult == cachedResult ? 0 : 1;
}

