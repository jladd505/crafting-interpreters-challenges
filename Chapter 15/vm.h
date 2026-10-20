#ifndef CHAPTER15_VM_H
#define CHAPTER15_VM_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  OP_CONSTANT,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_RETURN
} OpCode;

typedef struct {
  uint8_t code[128];
  int codeCount;
  double constants[64];
  int constantCount;
} Bytecode;

typedef struct {
  double* stack;
  size_t stackCount;
  size_t stackCapacity;
  size_t stackGrowths;
  size_t inPlaceUnaryOps;
  size_t inPlaceBinaryOps;
} VM;

void initBytecode(Bytecode* bytecode);
void emitOp(Bytecode* bytecode, OpCode op);
void emitConstant(Bytecode* bytecode, double value);
void initVM(VM* vm);
void freeVM(VM* vm);
void vmPush(VM* vm, double value);
double vmPop(VM* vm);
double vmRun(VM* vm, const Bytecode* bytecode);
const char* opName(OpCode op);

#endif

