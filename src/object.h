#ifndef object_h
#define object_h


#include "value.h"
#include <stddef.h>
#include <stdint.h>

typedef enum{
    OBJ_STRING
}ObjType;
#define OBJ_TYPE(p_obj) (p_obj->type)

struct Obj {
    ObjType type;
    struct Obj* next;
};
#define ALLOCATE_OBJ(ObjTipo, OBJ_TIPO) ((ObjTipo*)allocateObj(sizeof(ObjTipo), OBJ_TIPO))
Obj* allocateObj(size_t, ObjType);
void printObj(Value );

typedef struct {
    Obj base;
    int length;
    char* chars;
    uint32_t hash;
}ObjString;
#define IS_STRING(v) (isObjectType(v, OBJ_STRING))
#define AS_STRING(v) ((ObjString*)AS_OBJ(v))
#define AS_CSTRING(v) ( ((ObjString*)AS_OBJ(v))->chars )
ObjString* copyString(const char*, int);
ObjString* allocateString(char*, int, uint32_t);
ObjString* takeString(char*, int);


static inline bool isObjectType(Value v, ObjType type) {
    return IS_OBJ(v) && OBJ_TYPE(AS_OBJ(v)) == type;
}
#endif 