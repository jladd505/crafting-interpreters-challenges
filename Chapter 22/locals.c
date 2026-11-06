#include "locals.h"

#include <stdio.h>
#include <string.h>

static unsigned hashName(const char* name) {
  unsigned hash = 2166136261u;
  while (*name) { hash ^= (unsigned char)*name++; hash *= 16777619u; }
  return hash;
}

static size_t bucketFor(const char* name) { return hashName(name) & (LOCAL_BUCKETS - 1); }

void initLocalCompiler(LocalCompiler* compiler) {
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  compiler->error[0] = '\0';
  for (size_t i = 0; i < LOCAL_BUCKETS; i++) compiler->buckets[i] = -1;
}

void beginScope(LocalCompiler* compiler) { compiler->scopeDepth++; }

void endScope(LocalCompiler* compiler) {
  while (compiler->localCount > 0 &&
         compiler->locals[compiler->localCount - 1].depth == compiler->scopeDepth) {
    Local* local = &compiler->locals[compiler->localCount - 1];
    size_t bucket = bucketFor(local->name);
    compiler->buckets[bucket] = local->nextInBucket;
    compiler->localCount--;
  }
  if (compiler->scopeDepth > 0) compiler->scopeDepth--;
}

bool declareLocal(LocalCompiler* compiler, const char* name, LocalKind kind, size_t* slot) {
  if (compiler->localCount >= MAX_LOCALS) {
    snprintf(compiler->error, sizeof(compiler->error), "Too many local variables.");
    return false;
  }
  size_t bucket = bucketFor(name);
  for (int i = compiler->buckets[bucket]; i >= 0; i = compiler->locals[i].nextInBucket) {
    if (compiler->locals[i].depth < compiler->scopeDepth) break;
    if (strcmp(compiler->locals[i].name, name) == 0) {
      snprintf(compiler->error, sizeof(compiler->error), "Variable already declared in this scope.");
      return false;
    }
  }
  *slot = compiler->localCount;
  Local* local = &compiler->locals[compiler->localCount++];
  snprintf(local->name, sizeof(local->name), "%s", name);
  local->depth = compiler->scopeDepth;
  local->initialized = false;
  local->mutable = kind == LOCAL_VAR;
  local->nextInBucket = compiler->buckets[bucket];
  compiler->buckets[bucket] = (int)*slot;
  return true;
}

void defineLocal(LocalCompiler* compiler, size_t slot) {
  if (slot < compiler->localCount) compiler->locals[slot].initialized = true;
}

bool resolveLocal(LocalCompiler* compiler, const char* name, size_t* slot) {
  size_t bucket = bucketFor(name);
  for (int i = compiler->buckets[bucket]; i >= 0; i = compiler->locals[i].nextInBucket) {
    if (strcmp(compiler->locals[i].name, name) != 0) continue;
    if (!compiler->locals[i].initialized) {
      snprintf(compiler->error, sizeof(compiler->error), "Cannot read local variable in its own initializer.");
      return false;
    }
    *slot = (size_t)i;
    return true;
  }
  snprintf(compiler->error, sizeof(compiler->error), "Unknown local variable.");
  return false;
}

bool assignLocal(LocalCompiler* compiler, const char* name, size_t* slot) {
  if (!resolveLocal(compiler, name, slot)) return false;
  if (!compiler->locals[*slot].mutable) {
    snprintf(compiler->error, sizeof(compiler->error), "Cannot assign to immutable 'let' variable.");
    return false;
  }
  return true;
}

size_t encodeLocalInstruction(LocalOp shortOp, LocalOp longOp, size_t slot, unsigned char out[3]) {
  if (slot <= 255) {
    out[0] = (unsigned char)shortOp;
    out[1] = (unsigned char)slot;
    return 2;
  }
  if (slot > 65535) return 0;
  out[0] = (unsigned char)longOp;
  out[1] = (unsigned char)((slot >> 8) & 0xff);
  out[2] = (unsigned char)(slot & 0xff);
  return 3;
}

