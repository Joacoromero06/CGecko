#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>

int main() {
// INICIO
  initVM();
  Chunk chunk;
  initChunk(&chunk);

// HAND COMPILATION
  int constant = addConstant(&chunk, 3.14);
  writeChunk(&chunk, OP_CONST, 123);
  writeChunk(&chunk, constant, 123);
  
  constant = addConstant(&chunk, 2.36);
  writeChunk(&chunk, OP_CONST, 123);
  writeChunk(&chunk, constant, 123);
  
  writeChunk(&chunk, OP_SUMAR, 123);

  constant = addConstant(&chunk, 2.5);
  writeChunk(&chunk, OP_CONST, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_DIVIDIR, 123);
  writeChunk(&chunk, OP_NEGAR, 123);
  writeChunk(&chunk, OP_RETURN, 123);

// DEBUG BYTECODE EJECUCION
  disambleChunk(&chunk, "auto compilacion de: a + b / c");
  interpret(&chunk);

// CIERRE
  freeVM();
  freeChunk(&chunk);
  printf("\n");
  return 0;
}