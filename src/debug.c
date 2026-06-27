#include "debug.h"
#include "chunk.h"
#include "vm.h"
#include <stdio.h> 

void disambleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);
  for (int offset = 0; offset < chunk->count;) {
    offset = disambleInstruction(chunk, offset);
  }
}
int disambleInstruction(Chunk *chunk, int offset) {
  printf("ip:%04d ", offset);
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset-1])
    printf("   | ");
  else
    printf("%4d ", chunk->lines[offset]);

  switch (chunk->code[offset]) {
  case OP_RETURN: return simpleInstruction("OP_RETURN", offset);
  case OP_CONST: return constantInstruction("OP_CONST", chunk, offset);
  case OP_NIL: return simpleInstruction("OP_NIL", offset);
  
  case OP_FALSE: return simpleInstruction("OP_FALSE", offset);
  case OP_TRUE: return simpleInstruction("OP_TRUE", offset);
  case OP_NEGAR: return simpleInstruction("OP_NEGAR", offset);
  case OP_IGUALES: return  simpleInstruction("OP_IGUALES", offset);
  case OP_MAYOR: return simpleInstruction("OP_MAYOR", offset);
  case OP_MENOR: return simpleInstruction("OP_MENOR", offset);

  case OP_MENOS_UNARIO: return simpleInstruction("OP_MENOS_UNARIO", offset);
  case OP_SUMAR: return simpleInstruction("OP_SUMAR", offset);
  case OP_RESTAR: return simpleInstruction("OP_RESTAR", offset);
  case OP_MULTIPLICAR: return simpleInstruction("OP_MULTIPLICAR", offset);
  case OP_DIVIDIR: return simpleInstruction("OP_DIVIDIR", offset);  
  
  case OP_POP: return simpleInstruction("OP_POP", offset);
  case OP_PRINT: return simpleInstruction("OP_PRINT", offset);
  case OP_DECL_GLOBAL: return constantInstruction("OP_DECLARACION_GLOBAL", chunk, offset);
  case OP_GET_GLOBAL: return constantInstruction("OP_GET_GLOBAL", chunk, offset);
  case OP_SET_GLOBAL: return constantInstruction("OP_SET_GLOBAL", chunk, offset);
  case OP_GET_LOCAL: return indexInstruction("OP_GET_LOCAL", chunk, offset);
  case OP_SET_LOCAL: return indexInstruction("OP_SET_LOCAL", chunk, offset);
  case OP_JUMP: return jumpInstruction("OP_JUMP", 1, chunk, offset);
  case OP_JUMP_IF_FALSE: return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
  case OP_LOOP: return jumpInstruction("OP_LOOP", -1, chunk, offset);
  default: return RUNTIME_ERROR;
  };
}
int simpleInstruction(const char *instruction, int offset) {
  printf("%s\n", instruction);
  return offset + 1;
}
int constantInstruction(const char *instruction, Chunk* chunk, int offset) {
  int constant = chunk->code[offset + 1];
  printf("%-16s pstack:%04d '", instruction, constant);
  printValue(chunk->constants.values[constant]); 
  printf("'\n");
  return offset + 2;
}
int indexInstruction(const char * instruction, Chunk * chunk, int offset) {
  uint8_t local_index = chunk->code[offset + 1];
  printf("%-16s plocal:%04d \n", instruction, local_index);
  return offset+2;
}
int jumpInstruction(const char* instruction,int sign, Chunk* chunk, int offset){
  uint16_t jump = (uint16_t) (chunk->code[offset+1] << 8);
  jump |= chunk->code[offset+2];
  printf("%-16s %4d -> %d\n", instruction, offset, offset + 3 + sign*jump);
  return offset + 3;
}
