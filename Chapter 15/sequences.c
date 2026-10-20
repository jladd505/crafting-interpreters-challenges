#include "sequences.h"

static void finish(Bytecode* bytecode) { emitOp(bytecode, OP_RETURN); }

Bytecode expressionOne(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 1); emitConstant(&b, 2); emitOp(&b, OP_MULTIPLY);
  emitConstant(&b, 3); emitOp(&b, OP_ADD); finish(&b); return b;
}

Bytecode expressionTwo(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 1); emitConstant(&b, 2); emitConstant(&b, 3);
  emitOp(&b, OP_MULTIPLY); emitOp(&b, OP_ADD); finish(&b); return b;
}

Bytecode expressionThree(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 3); emitConstant(&b, 2); emitOp(&b, OP_SUBTRACT);
  emitConstant(&b, 1); emitOp(&b, OP_SUBTRACT); finish(&b); return b;
}

Bytecode expressionFour(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 1); emitConstant(&b, 2); emitConstant(&b, 3);
  emitOp(&b, OP_MULTIPLY); emitOp(&b, OP_ADD);
  emitConstant(&b, 4); emitConstant(&b, 5); emitOp(&b, OP_NEGATE);
  emitOp(&b, OP_DIVIDE); emitOp(&b, OP_SUBTRACT); finish(&b); return b;
}

Bytecode withoutNegate(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 4); emitConstant(&b, 3); emitConstant(&b, 0);
  emitConstant(&b, 2); emitOp(&b, OP_SUBTRACT); emitOp(&b, OP_MULTIPLY);
  emitOp(&b, OP_SUBTRACT); finish(&b); return b;
}

Bytecode withoutSubtract(void) {
  Bytecode b; initBytecode(&b);
  emitConstant(&b, 4); emitConstant(&b, 3); emitConstant(&b, 2);
  emitOp(&b, OP_NEGATE); emitOp(&b, OP_MULTIPLY); emitOp(&b, OP_NEGATE);
  emitOp(&b, OP_ADD); finish(&b); return b;
}

