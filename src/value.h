#ifndef value_h
#define value_h

#include <stdint.h>
#include <stdbool.h>

typedef struct Obj Obj;

typedef enum{
    VAL_BOOL,
    VAL_NUM,
    VAL_NIL,
    VAL_OBJ
} TValue;
typedef struct{
    TValue type; // nil aqui
    union{ 
        double numero; 
        bool booleano;
        Obj* objeto;
    }as;
} Value;

void printValue(Value);
bool equalsValue(Value, Value);
bool isFalsey(Value);
#define VAL_BOOL(b) ((Value){VAL_BOOL, .as.booleano=(b)})
#define VAL_NIL()   ((Value){VAL_NIL, .as.numero=0})
#define VAL_NUM(n)  ((Value){VAL_NUM, .as.numero=(n)}) 
#define VAL_OBJ(o)  ((Value){VAL_OBJ, .as.objeto=(Obj*)(o)})

#define IS_BOOL(v) ((v).type == VAL_BOOL)
#define IS_NIL(v)  ((v).type == VAL_NIL)
#define IS_NUM(v)  ((v).type == VAL_NUM)
#define IS_OBJ(v)  ((v).type == VAL_OBJ)

#define AS_BOOL(v) ((v).as.booleano)
#define AS_NUM(v)  ((v).as.numero)
#define AS_OBJ(v)  ((v).as.objeto)


typedef struct{
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray*);
void writeValueArray(ValueArray*, Value);
void freeValueArray(ValueArray*);

#endif