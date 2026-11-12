#include "natives.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static bool fail(Value* args, const char* message) {
  args[-1] = STRING_VALUE(message);
  return false;
}

static bool clockNative(int argCount, Value* args) {
  (void)argCount;
  args[-1] = NUMBER_VALUE((double)clock() / CLOCKS_PER_SEC);
  return true;
}

static bool sqrtNative(int argCount, Value* args) {
  (void)argCount;
  if (args[0].type != VALUE_NUMBER) return fail(args, "sqrt() expects a number.");
  if (args[0].as.number < 0) return fail(args, "sqrt() requires a non-negative number.");
  args[-1] = NUMBER_VALUE(sqrt(args[0].as.number));
  return true;
}

static bool powNative(int argCount, Value* args) {
  (void)argCount;
  if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER) {
    return fail(args, "pow() expects two numbers.");
  }
  args[-1] = NUMBER_VALUE(pow(args[0].as.number, args[1].as.number));
  return true;
}

static bool lengthNative(int argCount, Value* args) {
  (void)argCount;
  if (args[0].type != VALUE_STRING) return fail(args, "length() expects a string.");
  args[-1] = NUMBER_VALUE((double)strlen(args[0].as.string));
  return true;
}

const char* valueTypeName(Value value) {
  switch (value.type) {
    case VALUE_NIL: return "nil";
    case VALUE_BOOL: return "boolean";
    case VALUE_NUMBER: return "number";
    case VALUE_STRING: return "string";
  }
  return "unknown";
}

static bool typeOfNative(int argCount, Value* args) {
  (void)argCount;
  args[-1] = STRING_VALUE(valueTypeName(args[0]));
  return true;
}

static const Native natives[] = {
  {"clock", 0, clockNative},
  {"sqrt", 1, sqrtNative},
  {"pow", 2, powNative},
  {"length", 1, lengthNative},
  {"typeOf", 1, typeOfNative},
};

const Native* findNative(const char* name) {
  size_t count = sizeof(natives) / sizeof(natives[0]);
  for (size_t i = 0; i < count; i++) {
    if (strcmp(natives[i].name, name) == 0) return &natives[i];
  }
  return NULL;
}

bool callNative(const Native* native, int argCount, Value* callSlots,
                char* error, size_t errorCapacity) {
  if (native == NULL) {
    snprintf(error, errorCapacity, "Unknown native function.");
    return false;
  }
  if (argCount != native->arity) {
    snprintf(error, errorCapacity, "Expected %d arguments but got %d.", native->arity, argCount);
    return false;
  }

  Value* args = callSlots + 1;
  if (!native->function(argCount, args)) {
    const char* message = callSlots[0].type == VALUE_STRING
                            ? callSlots[0].as.string
                            : "Native function failed.";
    snprintf(error, errorCapacity, "%s", message);
    return false;
  }
  return true;
}

