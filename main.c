#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  int constantLong = addConstant(&chunk, 3.1456783947343243);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_CONSTANT_LONG, 123);
  writeChunk(&chunk, (constantLong >> 16) & 0xff, 123);
  writeChunk(&chunk, (constantLong >> 8) & 0xff, 123);
  writeChunk(&chunk, constantLong & 0xff, 123);

  writeChunk(&chunk, OP_RETURN, 123);

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
