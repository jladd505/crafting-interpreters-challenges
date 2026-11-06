#include "locals.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  LocalCompiler compiler;
  initLocalCompiler(&compiler);
  beginScope(&compiler);
  size_t outer;
  assert(declareLocal(&compiler, "value", LOCAL_VAR, &outer));
  assert(!resolveLocal(&compiler, "value", &outer));
  assert(strstr(compiler.error, "initializer") != NULL);
  defineLocal(&compiler, outer);
  assert(resolveLocal(&compiler, "value", &outer));

  beginScope(&compiler);
  size_t inner;
  assert(declareLocal(&compiler, "value", LOCAL_LET, &inner));
  defineLocal(&compiler, inner);
  assert(resolveLocal(&compiler, "value", &inner) && inner != outer);
  size_t assignment;
  assert(!assignLocal(&compiler, "value", &assignment));
  endScope(&compiler);
  assert(resolveLocal(&compiler, "value", &assignment) && assignment == outer);
  assert(assignLocal(&compiler, "value", &assignment));

  char name[48];
  for (int i = 0; i < 998; i++) {
    snprintf(name, sizeof(name), "local_%d", i);
    size_t slot;
    assert(declareLocal(&compiler, name, LOCAL_VAR, &slot));
    defineLocal(&compiler, slot);
  }
  assert(compiler.localCount == 999);
  unsigned char code[3];
  assert(encodeLocalInstruction(OP_GET_LOCAL, OP_GET_LOCAL_LONG, 200, code) == 2);
  assert(code[0] == OP_GET_LOCAL && code[1] == 200);
  assert(encodeLocalInstruction(OP_GET_LOCAL, OP_GET_LOCAL_LONG, 998, code) == 3);
  assert(code[0] == OP_GET_LOCAL_LONG && ((size_t)code[1] << 8 | code[2]) == 998);
  puts("chapter22: all tests passed");
  return 0;
}

