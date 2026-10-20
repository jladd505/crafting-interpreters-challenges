#include "vm.h"

#include <assert.h>
#include <stdlib.h>

void initBytecode(Bytecode* bytecode) {
  bytecode->codeCount = 0;
  bytecode->constantCount = 0;
}

void emitOp(Bytecode* bytecode, OpCode op) {
  assert(bytecode->codeCount < (int)sizeof(bytecode->code));
  bytecode->code[bytecode->codeCount++] = (uint8_t)op;
}

void emitConstant(Bytecode* bytecode, double value) {
  assert(bytecode->constantCount < 64);
  int index = bytecode->constantCount;
  bytecode->constants[bytecode->constantCount++] = value;
  emitOp(bytecode, OP_CONSTANT);
  bytecode->code[bytecode->codeCount++] = (uint8_t)index;
}

void initVM(VM* vm) {
  vm->stack = NULL;
  vm->stackCount = 0;
  vm->stackCapacity = 0;
  vm->stackGrowths = 0;
  vm->inPlaceUnaryOps = 0;
  vm->inPlaceBinaryOps = 0;
}

void freeVM(VM* vm) {
  free(vm->stack);
  initVM(vm);
}

void vmPush(VM* vm, double value) {
  if (vm->stackCount + 1 > vm->stackCapacity) {
    size_t capacity = vm->stackCapacity < 8 ? 8 : vm->stackCapacity * 2;
    double* grown = realloc(vm->stack, capacity * sizeof(double));
    if (grown == NULL) abort();
    vm->stack = grown;
    vm->stackCapacity = capacity;
    vm->stackGrowths++;
  }
  vm->stack[vm->stackCount++] = value;
}

double vmPop(VM* vm) {
  assert(vm->stackCount > 0);
  return vm->stack[--vm->stackCount];
}

static void binaryInPlace(VM* vm, OpCode op) {
  assert(vm->stackCount >= 2);
  double right = vm->stack[vm->stackCount - 1];
  double* left = &vm->stack[vm->stackCount - 2];
  switch (op) {
    case OP_ADD: *left += right; break;
    case OP_SUBTRACT: *left -= right; break;
    case OP_MULTIPLY: *left *= right; break;
    case OP_DIVIDE: *left /= right; break;
    default: abort();
  }
  vm->stackCount--;
  vm->inPlaceBinaryOps++;
}

double vmRun(VM* vm, const Bytecode* bytecode) {
  vm->stackCount = 0;
  int ip = 0;
  for (;;) {
    OpCode op = (OpCode)bytecode->code[ip++];
    switch (op) {
      case OP_CONSTANT: {
        uint8_t index = bytecode->code[ip++];
        vmPush(vm, bytecode->constants[index]);
        break;
      }
      case OP_NEGATE:
        assert(vm->stackCount > 0);
        vm->stack[vm->stackCount - 1] = -vm->stack[vm->stackCount - 1];
        vm->inPlaceUnaryOps++;
        break;
      case OP_ADD:
      case OP_SUBTRACT:
      case OP_MULTIPLY:
      case OP_DIVIDE:
        binaryInPlace(vm, op);
        break;
      case OP_RETURN:
        return vmPop(vm);
    }
  }
}

const char* opName(OpCode op) {
  switch (op) {
    case OP_CONSTANT: return "CONST";
    case OP_NEGATE: return "NEGATE";
    case OP_ADD: return "ADD";
    case OP_SUBTRACT: return "SUBTRACT";
    case OP_MULTIPLY: return "MULTIPLY";
    case OP_DIVIDE: return "DIVIDE";
    case OP_RETURN: return "RETURN";
  }
  return "UNKNOWN";
}

