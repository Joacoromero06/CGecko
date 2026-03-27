#ifndef memory_h
#define memory_h

#include <stddef.h>
#include "object.h"

#define GROW_CAPACITY(capacity) ((capacity) < 8) ? 8 : (capacity) * 2
#define GROW_ARRAY(type, pointer, oldCount, newCount) (type*) reallocate(pointer, (oldCount) * (sizeof(type)), (newCount) * (sizeof(type)))
#define FREE_ARRAY(type, pointer, oldCount) (type*)reallocate(pointer, (oldCount) * (sizeof(type)), 0)

void* reallocate(void*, size_t, size_t);

#define ALLOCATE(type, length) ((type*)reallocate(NULL, 0, sizeof(type)*length))
#define FREE(type, pointer) (reallocate(pointer, sizeof(type), 0))
void freeObjects();
void freeObject(Obj*);

#endif