#include "vm.h"

#include <math.h>

void initChunk(Chunk* chunk) { chunk->codeCount = 0; chunk->constantCount = 0; }
void emitByte(Chunk* chunk, unsigned char byte) { chunk->code[chunk->codeCount++] = byte; }

void emitConstant(Chunk* chunk, Value value) {
  size_t index = chunk->constantCount;
  chunk->constants[chunk->constantCount++] = value;
  emitByte(chunk, OP_CONSTANT);
  emitByte(chunk, (unsigned char)index);
}

void emitSubtractionWithoutSubtract(Chunk* chunk) {
  emitByte(chunk, OP_NEGATE);
  emitByte(chunk, OP_ADD);
}

void emitNegationWithoutNegate(Chunk* chunk, Value operand) {
  emitByte(chunk, OP_ZERO);
  emitConstant(chunk, operand);
  emitByte(chunk, OP_SUBTRACT);
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;
  switch (a.type) {
    case VAL_BOOL: return a.as.boolean == b.as.boolean;
    case VAL_NIL: return true;
    case VAL_NUMBER: return a.as.number == b.as.number;
  }
  return false;
}

static bool popNumber(Value* stack, size_t* count, double* value) {
  if (*count == 0 || stack[*count - 1].type != VAL_NUMBER) return false;
  *value = stack[--(*count)].as.number;
  return true;
}

bool runChunk(const Chunk* chunk, Value* result) {
  Value stack[64];
  size_t stackCount = 0;
  size_t ip = 0;
  while (ip < chunk->codeCount) {
    OpCode instruction = (OpCode)chunk->code[ip++];
    if (instruction == OP_CONSTANT) stack[stackCount++] = chunk->constants[chunk->code[ip++]];
    else if (instruction == OP_ZERO) stack[stackCount++] = NUMBER_VAL(0);
    else if (instruction == OP_ONE) stack[stackCount++] = NUMBER_VAL(1);
    else if (instruction == OP_NEGATE) {
      if (stackCount == 0 || stack[stackCount - 1].type != VAL_NUMBER) return false;
      stack[stackCount - 1].as.number = -stack[stackCount - 1].as.number;
    } else if (instruction == OP_ADD || instruction == OP_SUBTRACT || instruction == OP_LESS || instruction == OP_COMPARE) {
      double right, left;
      if (!popNumber(stack, &stackCount, &right) || !popNumber(stack, &stackCount, &left)) return false;
      if (instruction == OP_ADD) stack[stackCount++] = NUMBER_VAL(left + right);
      else if (instruction == OP_SUBTRACT) stack[stackCount++] = NUMBER_VAL(left - right);
      else if (instruction == OP_LESS) stack[stackCount++] = BOOL_VAL(left < right);
      else stack[stackCount++] = NUMBER_VAL((left > right) - (left < right));
    } else if (instruction == OP_EQUAL) {
      if (stackCount < 2) return false;
      Value right = stack[--stackCount];
      stack[stackCount - 1] = BOOL_VAL(valuesEqual(stack[stackCount - 1], right));
    } else if (instruction == OP_INCREMENT || instruction == OP_DECREMENT) {
      if (stackCount == 0 || stack[stackCount - 1].type != VAL_NUMBER) return false;
      stack[stackCount - 1].as.number += instruction == OP_INCREMENT ? 1 : -1;
    } else if (instruction == OP_COMPARE_ZERO) {
      if (stackCount == 0 || stack[stackCount - 1].type != VAL_NUMBER) return false;
      double number = stack[stackCount - 1].as.number;
      stack[stackCount - 1] = NUMBER_VAL((number > 0) - (number < 0));
    } else if (instruction == OP_RETURN) {
      if (stackCount != 1) return false;
      *result = stack[0];
      return true;
    } else return false;
  }
  return false;
}

