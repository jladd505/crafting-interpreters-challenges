#ifndef CHAPTER18_VM_H
#define CHAPTER18_VM_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { VAL_BOOL, VAL_NIL, VAL_NUMBER } ValueType;
typedef struct {
  ValueType type;
  union { bool boolean; double number; } as;
} Value;

#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = (value)}})
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = (value)}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})

typedef enum {
  OP_CONSTANT,
  OP_ZERO,
  OP_ONE,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_EQUAL,
  OP_LESS,
  OP_COMPARE,
  OP_INCREMENT,
  OP_DECREMENT,
  OP_COMPARE_ZERO,
  OP_RETURN
} OpCode;

typedef struct {
  unsigned char code[128];
  size_t codeCount;
  Value constants[32];
  size_t constantCount;
} Chunk;

void initChunk(Chunk* chunk);
void emitByte(Chunk* chunk, unsigned char byte);
void emitConstant(Chunk* chunk, Value value);
void emitSubtractionWithoutSubtract(Chunk* chunk);
void emitNegationWithoutNegate(Chunk* chunk, Value operand);
bool runChunk(const Chunk* chunk, Value* result);
bool valuesEqual(Value a, Value b);

#endif

