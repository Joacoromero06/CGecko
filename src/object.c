#include "object.h"
#include "table.h"
#include "memory.h"
#include "value.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "vm.h"


extern VM vm;
void printObj(Value v) {
    switch ( OBJ_TYPE(AS_OBJ(v)) ) {
    case OBJ_STRING: printf("%s", AS_CSTRING(v)); break;
    default: return;
    }
}
Obj* allocateObj(size_t size, ObjType type) {
    Obj* o = reallocate(NULL, 0, size);
    o->type = type;
    o->next = vm.objects;
    vm.objects = o;
    return o;
}

ObjString* copyString(const char * s, int n) {
    uint32_t h = hashString(s, n);

    ObjString* intern = findStringTable(&vm.strings, s, n, h);
    if (intern != NULL) return intern;
    
    char* ns = ALLOCATE(char, n+1);
    memcpy(ns, s, n+1);
    ns[n] = '\0';
    return allocateString(ns, n, h);
}
ObjString* allocateString(char* s, int n, uint32_t h){
    ObjString* o = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    o->chars = s;
    o->length = n;
    o->hash = h;

    setTable(&vm.strings, o, VAL_NIL());
    return o;
}
ObjString* takeString(char* s, int n) {
    uint32_t h = hashString(s, n);
    ObjString* intern = findStringTable(&vm.strings, s, n, h);
    if (intern != NULL) {
        FREE_ARRAY(char, s, n);
        return intern;
    }

    return allocateString(s, n, h);
}



