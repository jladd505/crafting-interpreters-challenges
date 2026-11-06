#include "control.h"

#include <stdio.h>
#include <string.h>

static bool emit(ControlChunk* chunk, ControlOp op, int operand) {
  if (chunk->count >= MAX_CODE) return false;
  chunk->code[chunk->count++] = (Instruction){op, operand};
  return true;
}

void initControlChunk(ControlChunk* chunk) { chunk->count = 0; }

bool compileSwitch(ControlChunk* chunk, int switchValue, const SwitchCase* cases,
                   size_t caseCount, bool hasDefault, int defaultResult) {
  initControlChunk(chunk);
  int endJumps[MAX_JUMPS];
  size_t endCount = 0;
  if (caseCount > MAX_JUMPS || !emit(chunk, BC_CONSTANT, switchValue)) return false;
  for (size_t i = 0; i < caseCount; i++) {
    if (!emit(chunk, BC_DUP, 0) || !emit(chunk, BC_CONSTANT, cases[i].value) ||
        !emit(chunk, BC_EQUAL, 0)) return false;
    size_t falseJump = chunk->count;
    if (!emit(chunk, BC_JUMP_IF_FALSE, -1) || !emit(chunk, BC_POP, 0) ||
        !emit(chunk, BC_RESULT, cases[i].result)) return false;
    endJumps[endCount++] = (int)chunk->count;
    if (!emit(chunk, BC_JUMP, -1)) return false;
    chunk->code[falseJump].operand = (int)chunk->count;
    if (!emit(chunk, BC_POP, 0)) return false;
  }
  if (hasDefault && !emit(chunk, BC_RESULT, defaultResult)) return false;
  size_t end = chunk->count;
  for (size_t i = 0; i < endCount; i++) chunk->code[endJumps[i]].operand = (int)end;
  return emit(chunk, BC_POP, 0) && emit(chunk, BC_RETURN, 0);
}

bool runControlChunk(const ControlChunk* chunk, int* result) {
  int stack[128];
  size_t stackCount = 0;
  bool hasResult = false;
  size_t ip = 0;
  while (ip < chunk->count) {
    Instruction instruction = chunk->code[ip++];
    switch (instruction.op) {
      case BC_CONSTANT: stack[stackCount++] = instruction.operand; break;
      case BC_DUP:
        if (stackCount == 0) return false;
        stack[stackCount] = stack[stackCount - 1]; stackCount++; break;
      case BC_EQUAL:
        if (stackCount < 2) return false;
        stack[stackCount - 2] = stack[stackCount - 2] == stack[stackCount - 1];
        stackCount--; break;
      case BC_JUMP_IF_FALSE:
        if (stackCount == 0) return false;
        if (!stack[stackCount - 1]) ip = (size_t)instruction.operand;
        break;
      case BC_JUMP: ip = (size_t)instruction.operand; break;
      case BC_POP:
        if (stackCount == 0) return false;
        stackCount--; break;
      case BC_RESULT: *result = instruction.operand; hasResult = true; break;
      case BC_LOOP: ip = (size_t)instruction.operand; break;
      case BC_RETURN: return hasResult;
    }
  }
  return false;
}

void initLoopCompiler(LoopCompiler* compiler) {
  initControlChunk(&compiler->chunk);
  compiler->loopStart = -1;
  compiler->loopScopeDepth = 0;
  compiler->localCount = 0;
}

bool emitContinue(LoopCompiler* compiler) {
  if (compiler->loopStart < 0) return false;
  for (size_t i = compiler->localCount; i > 0; i--) {
    if (compiler->localDepths[i - 1] <= compiler->loopScopeDepth) break;
    if (!emit(&compiler->chunk, BC_POP, 0)) return false;
  }
  return emit(&compiler->chunk, BC_LOOP, compiler->loopStart);
}

bool betweenJoin(const char* const* items, size_t count, const char* separator,
                 char* output, size_t capacity) {
  if (capacity == 0) return false;
  output[0] = '\0';
  size_t used = 0;
  for (size_t i = 0; i < count; i++) {
    const char* part = i == 0 ? items[i] : separator;
    const char* second = i == 0 ? NULL : items[i];
    size_t firstLength = strlen(part);
    size_t secondLength = second == NULL ? 0 : strlen(second);
    if (used + firstLength + secondLength + 1 > capacity) return false;
    memcpy(output + used, part, firstLength); used += firstLength;
    if (second != NULL) { memcpy(output + used, second, secondLength); used += secondLength; }
    output[used] = '\0';
  }
  return true;
}

