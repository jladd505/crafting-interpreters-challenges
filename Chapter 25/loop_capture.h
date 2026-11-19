#ifndef CHAPTER25_LOOP_CAPTURE_H
#define CHAPTER25_LOOP_CAPTURE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct CapturedCell CapturedCell;

typedef struct {
  CapturedCell* cell;
} LoopClosure;

bool loop_closures_create_fresh(int first, size_t count, LoopClosure* out);
int loop_closure_get(const LoopClosure* closure);
void loop_closure_set(LoopClosure* closure, int value);
void loop_closure_destroy(LoopClosure* closure);

#endif

