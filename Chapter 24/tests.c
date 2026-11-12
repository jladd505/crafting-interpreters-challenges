#include "ip_cache.h"
#include "natives.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void testInstructionPointers(void) {
  const uint8_t code[] = {BENCH_INCREMENT, BENCH_MIX, BENCH_INCREMENT, BENCH_HALT};
  const uint8_t* frameEnd = NULL;
  const uint8_t* cachedEnd = NULL;
  uint64_t frameResult = runFrameIp(code, 100, &frameEnd);
  uint64_t cachedResult = runCachedIp(code, 100, &cachedEnd);
  assert(frameResult == cachedResult);
  assert(frameEnd == code + sizeof(code));
  assert(cachedEnd == code + sizeof(code));
}

static void testNatives(void) {
  char error[128];
  Value slots[3] = {NIL_VALUE, NUMBER_VALUE(9), NIL_VALUE};
  assert(callNative(findNative("clock"), 0, slots, error, sizeof(error)));
  assert(slots[0].type == VALUE_NUMBER && slots[0].as.number >= 0);

  slots[0] = NIL_VALUE;
  slots[1] = NUMBER_VALUE(9);
  assert(callNative(findNative("sqrt"), 1, slots, error, sizeof(error)));
  assert(slots[0].type == VALUE_NUMBER && slots[0].as.number == 3);

  slots[0] = NIL_VALUE;
  slots[1] = NUMBER_VALUE(2);
  slots[2] = NUMBER_VALUE(8);
  assert(callNative(findNative("pow"), 2, slots, error, sizeof(error)));
  assert(slots[0].as.number == 256);

  slots[0] = NIL_VALUE;
  slots[1] = STRING_VALUE("hello");
  assert(callNative(findNative("length"), 1, slots, error, sizeof(error)));
  assert(slots[0].as.number == 5);

  slots[0] = NIL_VALUE;
  slots[1] = BOOL_VALUE(true);
  assert(callNative(findNative("typeOf"), 1, slots, error, sizeof(error)));
  assert(strcmp(slots[0].as.string, "boolean") == 0);

  slots[0] = NIL_VALUE;
  assert(!callNative(findNative("pow"), 0, slots, error, sizeof(error)));
  assert(strstr(error, "Expected 2") != NULL);

  slots[0] = NIL_VALUE;
  slots[1] = STRING_VALUE("bad");
  assert(!callNative(findNative("sqrt"), 1, slots, error, sizeof(error)));
  assert(strstr(error, "expects a number") != NULL);

  slots[0] = NIL_VALUE;
  slots[1] = NUMBER_VALUE(-1);
  assert(!callNative(findNative("sqrt"), 1, slots, error, sizeof(error)));
  assert(strstr(error, "non-negative") != NULL);
  assert(findNative("missing") == NULL);
}

int main(void) {
  testInstructionPointers();
  testNatives();
  puts("chapter24: all tests passed");
  return 0;
}

