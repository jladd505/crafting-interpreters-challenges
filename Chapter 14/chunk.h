#ifndef CHAPTER14_CHUNK_H
#define CHAPTER14_CHUNK_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_RETURN
} OpCode;

typedef struct {
  int offset;
  int line;
} LineStart;

typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  int constantCount;
  int constantCapacity;
  double* constants;
  int lineCount;
  int lineCapacity;
  LineStart* lines;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, double value);
void writeConstant(Chunk* chunk, double value, int line);
int getLine(const Chunk* chunk, int instruction);
uint32_t constantIndexAt(const Chunk* chunk, int offset);
int disassembleInstruction(const Chunk* chunk, int offset, FILE* output);

#endif

