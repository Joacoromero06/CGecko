#include "value.h"
#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "object.h"

void initValueArray(ValueArray *arr) {
  arr->capacity = 0;
  arr->count = 0;
  arr->values = NULL;
}
void writeValueArray(ValueArray *arr, Value v) {
  if (arr->capacity == arr->count) {
    int oldCapacity = arr->capacity;
    arr->capacity = GROW_CAPACITY(oldCapacity);
    arr->values = GROW_ARRAY(Value, arr->values, oldCapacity, arr->capacity);
  }
  arr->values[arr->count++] = v;
}
void freeValueArray(ValueArray *arr) {
  FREE_ARRAY(Value, arr->values, arr->count);
  initValueArray(arr);
}
void printValue(Value v) { 
  switch (v.type) {
  case VAL_BOOL: printf(AS_BOOL(v) ? "true" : "false"); break;
  case VAL_NIL: printf("nil"); break;
  case VAL_NUM: printf("%g", AS_NUM(v)); break;
  case VAL_OBJ: printObj(v); break;
  default: return;
  }
}
bool isFalsey(Value v) {
  return ( IS_NIL(v) || (IS_BOOL(v) && !AS_BOOL(v)) );
}
bool equalsValue(Value a, Value b){
  if (a.type != b.type) return false;
  switch (a.type) {
  case VAL_NIL: return true;
  case VAL_NUM: return AS_NUM(a) == AS_NUM(b);
  case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
  case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
  default: return false;
  }
}