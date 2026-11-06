#ifndef CHAPTER23_CONTROL_H
#define CHAPTER23_CONTROL_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_CODE 512
#define MAX_JUMPS 128

typedef enum {
  BC_CONSTANT,
  BC_DUP,
  BC_EQUAL,
  BC_JUMP_IF_FALSE,
  BC_JUMP,
  BC_POP,
  BC_RESULT,
  BC_LOOP,
  BC_RETURN
} ControlOp;

typedef struct { ControlOp op; int operand; } Instruction;
typedef struct { Instruction code[MAX_CODE]; size_t count; } ControlChunk;
typedef struct { int value; int result; } SwitchCase;

void initControlChunk(ControlChunk* chunk);
bool compileSwitch(ControlChunk* chunk, int switchValue, const SwitchCase* cases,
                   size_t caseCount, bool hasDefault, int defaultResult);
bool runControlChunk(const ControlChunk* chunk, int* result);

typedef struct {
  ControlChunk chunk;
  int loopStart;
  int loopScopeDepth;
  int localDepths[64];
  size_t localCount;
} LoopCompiler;

void initLoopCompiler(LoopCompiler* compiler);
bool emitContinue(LoopCompiler* compiler);
bool betweenJoin(const char* const* items, size_t count, const char* separator,
                 char* output, size_t capacity);

#endif

