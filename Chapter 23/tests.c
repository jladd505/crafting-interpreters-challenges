#include "control.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  SwitchCase cases[] = {{1, 100}, {2, 200}, {3, 300}};
  ControlChunk chunk;
  int result = 0;
  assert(compileSwitch(&chunk, 2, cases, 3, true, 999));
  assert(runControlChunk(&chunk, &result) && result == 200);
  assert(compileSwitch(&chunk, 7, cases, 3, true, 999));
  assert(runControlChunk(&chunk, &result) && result == 999);
  assert(compileSwitch(&chunk, 7, cases, 3, false, 0));
  assert(!runControlChunk(&chunk, &result));

  LoopCompiler compiler;
  initLoopCompiler(&compiler);
  assert(!emitContinue(&compiler));
  compiler.loopStart = 12;
  compiler.loopScopeDepth = 1;
  compiler.localDepths[0] = 1;
  compiler.localDepths[1] = 2;
  compiler.localDepths[2] = 3;
  compiler.localCount = 3;
  assert(emitContinue(&compiler));
  assert(compiler.chunk.count == 3);
  assert(compiler.chunk.code[0].op == BC_POP && compiler.chunk.code[1].op == BC_POP);
  assert(compiler.chunk.code[2].op == BC_LOOP && compiler.chunk.code[2].operand == 12);

  const char* items[] = {"red", "green", "blue"};
  char output[64];
  assert(betweenJoin(items, 3, ", ", output, sizeof(output)));
  assert(strcmp(output, "red, green, blue") == 0);
  assert(betweenJoin(items, 0, ", ", output, sizeof(output)) && strcmp(output, "") == 0);
  puts("chapter23: all tests passed");
  return 0;
}

