#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  if (chunk->lineCapacity < chunk->lineCount + 1) {
    int oldLineCapacity = chunk->lineCapacity;
    chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
    chunk->lines = GROW_ARRAY(LineRun, chunk->lines, oldLineCapacity, chunk->lineCapacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->count++;

  // chunk->lineCount counts the number of LineRun entries
  // LineRun.count counts the instructions
  if (chunk->lineCount == 0) {
    LineRun firstRun = {line, 1};
    chunk->lines[0] = firstRun;
    chunk->lineCount++;
  } else if (chunk->lines[chunk->lineCount - 1].line == line) {
    chunk->lines[chunk->lineCount - 1].count++;
  } else {
    LineRun newRun = {line, 1};
    chunk->lines[chunk->lineCount] = newRun;
    chunk->lineCount++;
  }
}

void writeConstant(Chunk* chunk, Value value, int line) {
  int constant = addConstant(chunk, value);

  if (constant <= 255) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)constant, line);
  } else {
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    writeChunk(chunk, (constant >> 16) & 0xff, line);
    writeChunk(chunk, (constant >> 8) & 0xff, line);
    writeChunk(chunk, constant & 0xff, line);
  }
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int instructionIndex) {
  int instructionsSeen = 0;

  for (int run = 0; run < chunk->lineCount; run++) {
    instructionsSeen += chunk->lines[run].count;
    if (instructionIndex < instructionsSeen) {
      return chunk->lines[run].line;
    }
  }

  return -1;
}
