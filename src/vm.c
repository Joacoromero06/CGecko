#include "vm.h"
#include "chunk.h"
#include "debug.h" 
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "compiler.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

static void concatenate();
// ===GLOBALES===
VM vm;

void initVM() { resetStack(); vm.objects =NULL; initTable(&vm.strings); initTable(&vm.globals); }
void freeVM() { freeTable(&vm.globals); freeTable(&vm.strings); resetStack(); freeObjects(); }

//===CONSTANT STACK MANAGERS===
static void resetStack() { vm.stackTop = vm.stack; }
static void push(Value v) {
  *vm.stackTop = v;
  vm.stackTop++;
}
static Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

// ===COMPLETE PIPELINE===
InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)){
    freeChunk(&chunk);
    return COMPILE_ERROR;
  }
  vm.chunk = &chunk;
  vm.ip = chunk.code;

  InterpretResult r = run();
  freeChunk(&chunk);
  return r;
}

// ===MAIN INTERPRETING===
static InterpretResult run() {
// MACROS DE RUN
#define READ_ADVANCE_BYTE() (*vm.ip++)
#define READ_ADVANCE_CONSTANT()                                                \
  (vm.chunk->constants.values[READ_ADVANCE_BYTE()])
#define READ_STRING() ( AS_STRING(READ_ADVANCE_CONSTANT()) )
#define READ_SHORT() (vm.ip += 2, (uint16_t) ((vm.ip[-2] << 8) | (vm.ip[-1])))
#define OP_BINARIA(VAL_TYPE, op) \
  do { \
    if (!IS_NUM(peek(1)) || !IS_NUM(peek(0))){ \
      runTimeError("Esperaba operandos numericos"); \
      return RUNTIME_ERROR; \
    } \
    double b = AS_NUM(pop()); \
    double a = AS_NUM(pop()); \
    push(VAL_TYPE(a op b)); \
  } while (false) 

  // RUNTIME LOOP
  for (;;) {
// DEBUGGING
#ifdef DEBUG_TRACE_EXECUTION
    printf("\t");
    for (Value *p = vm.stack; p < vm.stackTop; p++) {
      printf(" [");
      printValue(*p);
      printf("] ");
    }
    printf("\n");
    disambleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

    // DECODING y EXECUTING INSTRUCTIONS
    switch (READ_ADVANCE_BYTE()) {
    case OP_RETURN: {
      return INTERPRET_OK;
    }
    case OP_CONST: {
      Value constant = READ_ADVANCE_CONSTANT();
      push(constant);
      break;
    }
    case OP_IGUALES: {
      Value b = pop();
      Value a = pop();
      push(VAL_BOOL(equalsValue(a, b)));
      break;
    }
    case OP_SUMAR: {
      if ( IS_STRING(peek(1)) &&  IS_STRING(peek(0)) )
        concatenate();
      else if ( IS_NUM(peek(1)) && IS_NUM(peek(0))){
        double b = AS_NUM(pop());
        double a = AS_NUM(pop());
        push(VAL_NUM(a + b));
      } else {
        runTimeError("Esperaba dos operandos numericos o strings");
        return RUNTIME_ERROR;
      }
      break;
    }
    case OP_MENOS_UNARIO:{
      if ( !IS_NUM(peek(0)) ){
        runTimeError("Operando debe ser un numero");
        return RUNTIME_ERROR;
      }
      push(VAL_NUM( -AS_NUM(pop()) ) );
      break;
    }
    case OP_DECL_GLOBAL: {
      ObjString* name = READ_STRING();
      if (!setTable(&vm.globals, name, peek(0))) {
        runTimeError("Intento de declaracion de variable global ya declarada");
        return RUNTIME_ERROR;
      }
      pop();
      break;
    }
    case OP_GET_GLOBAL: {
      ObjString* name = READ_STRING();
      Value v;
      if (!getTable(&vm.globals, name, &v)) {
        runTimeError("Referencia a variable no declarada.");
        return RUNTIME_ERROR;
      }
      push(v);
      break;
    }
    case OP_SET_GLOBAL: {
      ObjString* n = READ_STRING();
      if (setTable(&vm.globals, n, peek(0))) {
        deleteTable(&vm.globals, n);
        runTimeError("Intento de asignacion a variable no declarada.");
        return RUNTIME_ERROR;
      }
      break;
    }
    case OP_GET_LOCAL: {
      uint8_t slot = READ_ADVANCE_BYTE();
      push(vm.stack[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = READ_ADVANCE_BYTE();
      vm.stack[slot] = peek(0);
      break;
    }

    case OP_NIL: push(VAL_NIL()); break;
    case OP_TRUE: push(VAL_BOOL(true)); break;
    case OP_FALSE: push(VAL_BOOL(false)); break;
    case OP_NEGAR: push( VAL_BOOL( isFalsey(pop()) ) ); break;
    case OP_MAYOR: OP_BINARIA(VAL_BOOL, >); break;
    case OP_MENOR: OP_BINARIA(VAL_BOOL, <); break;

    case OP_RESTAR: OP_BINARIA(VAL_NUM, -); break;
    case OP_MULTIPLICAR: OP_BINARIA(VAL_NUM, *); break;
    case OP_DIVIDIR: OP_BINARIA(VAL_NUM, /); break;
    
    case OP_POP: pop(); break;
    case OP_PRINT: printValue(pop()); printf("\n"); break;
    
    case OP_JUMP_IF_FALSE: {
      uint16_t offset = READ_SHORT();
      if (isFalsey(peek(0))) vm.ip += offset;
      break;
    }

    case OP_JUMP: {
      uint16_t offset = READ_SHORT();
      vm.ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = READ_SHORT();
      vm.ip -= offset;
      break;
    }
  }
}

#undef READ_ADVANCE_BYTE
#undef READ_ADVANCE_CONSTANT
#undef READ_STRING
#undef READ_SHORT
}
static void runTimeError(const char * format, ...){
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args); fputs("\n",stderr);

  size_t instruction = vm.ip-vm.chunk->code - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[ERROR] linea %2d\n", line);

  resetStack();
}
static Value peek(int distance){
  return vm.stackTop[-1-distance];
}
static void concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());
  int length = b->length + a->length;
  char* cs = ALLOCATE(char, length+1);
  memcpy(cs, a->chars, a->length);
  memcpy(cs + a->length, b->chars, b->length);
  cs[length] = '\0';
  ObjString* r = takeString(cs, length);
  push(VAL_OBJ(r));
}
