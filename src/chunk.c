#include "chunk.h"
#include "memory.h"
#include "value.h"
#include <stdint.h>
#include <stdlib.h>

void initChunk(Chunk *chunk) {
  chunk->capacity = 0;
  chunk->count = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}
void writeChunk(Chunk *chunk, uint8_t byte, int line) {
  if (chunk->capacity == chunk->count) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    chunk->code =
        GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }
  chunk->lines[chunk->count] = line;
  chunk->code[chunk->count++] = byte;
}

void freeChunk(Chunk *chunk) {
  freeValueArray(&chunk->constants);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  initChunk(chunk);
}
int addConstant(Chunk *chunk, Value v) {
  writeValueArray(&chunk->constants, v);
  return chunk->constants.count - 1;
}