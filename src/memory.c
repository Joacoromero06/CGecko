#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "vm.h"
extern VM vm;

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }
  void *result = realloc(pointer, newSize);
  if (!result)
    exit(1);
  return result;
}
void freeObjects() {
  Obj* o = vm.objects;
  while (o != NULL) {
    Obj* next = o->next;
    freeObject(o);
    o = next;
  }

}
void freeObject(Obj * o) {
  switch (o->type) {
  case OBJ_STRING: {
    ObjString* os = (ObjString*) o;
    FREE_ARRAY(char, os->chars, os->length+1);
    FREE(ObjString, o); 
    break;
  }
  default: return;
  }
}