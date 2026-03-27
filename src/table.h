#ifndef gecko_table_h
#define gecko_table_h

#include "object.h"
#include <stdint.h>
#define MAX_LOAD_FACTOR 0.75
typedef struct {
    ObjString* key;
    Value value;
} Entry;
typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;
void initTable(Table*);
void freeTable(Table*);
bool setTable(Table*, ObjString*, Value);
bool getTable(Table*, ObjString*, Value*);
bool deleteTable(Table*, ObjString*);

// INTERNING STRINGS
ObjString* findStringTable(Table*, const char*, int, uint32_t);


void tableAddAll(Table*, Table*);

Entry* findEntry(Entry*, int, ObjString*);
void adjustCapacty(Table*, int);

uint16_t hashString(const char*, int);


#endif