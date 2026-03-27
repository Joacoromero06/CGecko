#ifndef vm_h
#define vm_h

#include "chunk.h"
#include "table.h"
#include "value.h"
#include <stdint.h>

typedef enum {
    INTERPRET_OK,
    COMPILE_ERROR,
    RUNTIME_ERROR
} InterpretResult;

#define VM_STACK_SIZE 256
typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[VM_STACK_SIZE];
    Value* stackTop;
    Obj* objects;
    Table strings;
    Table globals;
}VM;

void initVM();
void freeVM();
static void resetStack();
static void push(Value);
static Value pop();

InterpretResult interpret(const char*);
static InterpretResult run();
static void runTimeError(const char* format, ...);
static Value peek(int);

#endif 