#include "loop_capture.h"

#include <stdlib.h>

struct CapturedCell {
  int value;
};

bool loop_closures_create_fresh(int first, size_t count, LoopClosure* out) {
  for (size_t i = 0; i < count; i++) out[i].cell = NULL;

  for (size_t i = 0; i < count; i++) {
    CapturedCell* iteration_variable = malloc(sizeof(*iteration_variable));
    if (iteration_variable == NULL) {
      for (size_t j = 0; j < i; j++) loop_closure_destroy(&out[j]);
      return false;
    }

    iteration_variable->value = first + (int)i;
    out[i].cell = iteration_variable;
  }
  return true;
}

int loop_closure_get(const LoopClosure* closure) {
  return closure->cell->value;
}

void loop_closure_set(LoopClosure* closure, int value) {
  closure->cell->value = value;
}

void loop_closure_destroy(LoopClosure* closure) {
  free(closure->cell);
  closure->cell = NULL;
}

