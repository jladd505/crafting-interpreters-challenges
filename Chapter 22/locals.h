#ifndef CHAPTER22_LOCALS_H
#define CHAPTER22_LOCALS_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LOCALS 4096
#define LOCAL_BUCKETS 8192

typedef enum { LOCAL_VAR, LOCAL_LET } LocalKind;
typedef enum { OP_GET_LOCAL = 1, OP_GET_LOCAL_LONG, OP_SET_LOCAL, OP_SET_LOCAL_LONG } LocalOp;

typedef struct {
  char name[48];
  int depth;
  bool initialized;
  bool mutable;
  int nextInBucket;
} Local;

typedef struct {
  Local locals[MAX_LOCALS];
  size_t localCount;
  int scopeDepth;
  int buckets[LOCAL_BUCKETS];
  char error[128];
} LocalCompiler;

void initLocalCompiler(LocalCompiler* compiler);
void beginScope(LocalCompiler* compiler);
void endScope(LocalCompiler* compiler);
bool declareLocal(LocalCompiler* compiler, const char* name, LocalKind kind, size_t* slot);
void defineLocal(LocalCompiler* compiler, size_t slot);
bool resolveLocal(LocalCompiler* compiler, const char* name, size_t* slot);
bool assignLocal(LocalCompiler* compiler, const char* name, size_t* slot);
size_t encodeLocalInstruction(LocalOp shortOp, LocalOp longOp, size_t slot, unsigned char out[3]);

#endif

