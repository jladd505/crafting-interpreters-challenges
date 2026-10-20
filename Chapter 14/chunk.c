#include "chunk.h"

#include <stdlib.h>

static int growCapacity(int capacity) { return capacity < 8 ? 8 : capacity * 2; }

static void* checkedRealloc(void* pointer, size_t size) {
  void* result = realloc(pointer, size);
  if (result == NULL && size > 0) abort();
  return result;
}

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->constantCount = 0;
  chunk->constantCapacity = 0;
  chunk->constants = NULL;
  chunk->lineCount = 0;
  chunk->lineCapacity = 0;
  chunk->lines = NULL;
}

void freeChunk(Chunk* chunk) {
  free(chunk->code);
  free(chunk->constants);
  free(chunk->lines);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->count + 1 > chunk->capacity) {
    chunk->capacity = growCapacity(chunk->capacity);
    chunk->code = checkedRealloc(chunk->code, (size_t)chunk->capacity * sizeof(uint8_t));
  }
  chunk->code[chunk->count++] = byte;

  if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount - 1].line == line) return;
  if (chunk->lineCount + 1 > chunk->lineCapacity) {
    chunk->lineCapacity = growCapacity(chunk->lineCapacity);
    chunk->lines = checkedRealloc(
        chunk->lines, (size_t)chunk->lineCapacity * sizeof(LineStart));
  }
  chunk->lines[chunk->lineCount++] = (LineStart){chunk->count - 1, line};
}

int addConstant(Chunk* chunk, double value) {
  if (chunk->constantCount + 1 > chunk->constantCapacity) {
    chunk->constantCapacity = growCapacity(chunk->constantCapacity);
    chunk->constants = checkedRealloc(
        chunk->constants, (size_t)chunk->constantCapacity * sizeof(double));
  }
  chunk->constants[chunk->constantCount] = value;
  return chunk->constantCount++;
}

void writeConstant(Chunk* chunk, double value, int line) {
  int index = addConstant(chunk, value);
  if (index < 256) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)index, line);
    return;
  }
  if (index >= (1 << 24)) abort();
  writeChunk(chunk, OP_CONSTANT_LONG, line);
  writeChunk(chunk, (uint8_t)(index & 0xff), line);
  writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
  writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
}

int getLine(const Chunk* chunk, int instruction) {
  if (instruction < 0 || instruction >= chunk->count || chunk->lineCount == 0) return -1;
  int low = 0;
  int high = chunk->lineCount - 1;
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (chunk->lines[mid].offset <= instruction &&
        (mid == chunk->lineCount - 1 || chunk->lines[mid + 1].offset > instruction)) {
      return chunk->lines[mid].line;
    }
    if (chunk->lines[mid].offset > instruction) high = mid - 1;
    else low = mid + 1;
  }
  return -1;
}

uint32_t constantIndexAt(const Chunk* chunk, int offset) {
  if (chunk->code[offset] == OP_CONSTANT) return chunk->code[offset + 1];
  if (chunk->code[offset] == OP_CONSTANT_LONG) {
    return (uint32_t)chunk->code[offset + 1] |
           ((uint32_t)chunk->code[offset + 2] << 8) |
           ((uint32_t)chunk->code[offset + 3] << 16);
  }
  return UINT32_MAX;
}

int disassembleInstruction(const Chunk* chunk, int offset, FILE* output) {
  fprintf(output, "%04d %4d ", offset, getLine(chunk, offset));
  switch (chunk->code[offset]) {
    case OP_CONSTANT: {
      uint32_t index = constantIndexAt(chunk, offset);
      fprintf(output, "OP_CONSTANT %u %.14g\n", index, chunk->constants[index]);
      return offset + 2;
    }
    case OP_CONSTANT_LONG: {
      uint32_t index = constantIndexAt(chunk, offset);
      fprintf(output, "OP_CONSTANT_LONG %u %.14g\n", index, chunk->constants[index]);
      return offset + 4;
    }
    case OP_RETURN:
      fputs("OP_RETURN\n", output);
      return offset + 1;
    default:
      fputs("UNKNOWN\n", output);
      return offset + 1;
  }
}

