#include "globals.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  GlobalStore store;
  initGlobalStore(&store);
  unsigned char first, again, later;
  assert(globalSlot(&store, "breakfast", &first));
  assert(globalSlot(&store, "breakfast", &again));
  assert(first == again && store.pool.count == 1);

  double value;
  assert(!getGlobal(&store, first, &value));
  defineGlobal(&store, first, 10);
  assert(getGlobal(&store, first, &value) && value == 10);
  assert(setGlobal(&store, first, 12));
  assert(getGlobal(&store, first, &value) && value == 12);

  assert(globalSlot(&store, "declaredLater", &later));
  assert(!setGlobal(&store, later, 3));
  defineGlobal(&store, later, 3);
  assert(getGlobal(&store, later, &value) && value == 3);
  freeGlobalStore(&store);
  puts("chapter21: all tests passed");
  return 0;
}

