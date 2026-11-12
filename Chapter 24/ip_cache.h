#ifndef CHAPTER24_IP_CACHE_H
#define CHAPTER24_IP_CACHE_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  BENCH_INCREMENT,
  BENCH_MIX,
  BENCH_HALT
} BenchOp;

typedef struct {
  const uint8_t* ip;
} BenchFrame;

uint64_t runFrameIp(const uint8_t* code, size_t rounds, const uint8_t** finalIp);
uint64_t runCachedIp(const uint8_t* code, size_t rounds, const uint8_t** finalIp);

#endif

