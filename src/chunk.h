#ifndef chunk_h
#define chunk_h

#include "value.h"
#include <stdint.h>


typedef enum{
    OP_RETURN,
//expressions
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_CONST,
    OP_MENOS_UNARIO,
    OP_SUMAR,
    OP_RESTAR,
    OP_MULTIPLICAR,
    OP_DIVIDIR,
    OP_NEGAR,
    OP_IGUALES,
    OP_MAYOR,
    OP_MENOR,
//statements
    OP_PRINT,
    OP_POP,
    OP_DECL_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,

//control-flow
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP
} Opcode ;

typedef struct{
    int capacity;
    int count;
    uint8_t* code;
    int* lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk*);
void writeChunk(Chunk*, uint8_t, int);
void freeChunk(Chunk*);

int addConstant(Chunk*, Value);
#endif
