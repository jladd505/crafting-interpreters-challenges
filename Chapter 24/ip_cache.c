#include "ip_cache.h"

static uint64_t mix(uint64_t value) {
  value ^= value >> 13;
  value *= UINT64_C(0x9e3779b97f4a7c15);
  return value ^ (value >> 29);
}

uint64_t runFrameIp(const uint8_t* code, size_t rounds, const uint8_t** finalIp) {
  BenchFrame frame = {code};
  uint64_t accumulator = 1;
  for (size_t round = 0; round < rounds; round++) {
    frame.ip = code;
    for (;;) {
      switch (*frame.ip++) {
        case BENCH_INCREMENT: accumulator += 1; break;
        case BENCH_MIX: accumulator = mix(accumulator); break;
        case BENCH_HALT:
          if (finalIp != NULL) *finalIp = frame.ip;
          goto nextRound;
      }
    }
nextRound:
    accumulator ^= round;
  }
  return accumulator;
}

uint64_t runCachedIp(const uint8_t* code, size_t rounds, const uint8_t** finalIp) {
  BenchFrame frame = {code};
  uint64_t accumulator = 1;
  for (size_t round = 0; round < rounds; round++) {
    register const uint8_t* ip = code;
    for (;;) {
      switch (*ip++) {
        case BENCH_INCREMENT: accumulator += 1; break;
        case BENCH_MIX: accumulator = mix(accumulator); break;
        case BENCH_HALT:
          frame.ip = ip;
          if (finalIp != NULL) *finalIp = frame.ip;
          goto nextRound;
      }
    }
nextRound:
    accumulator ^= round;
  }
  return accumulator;
}

