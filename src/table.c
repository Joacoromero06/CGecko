#include "table.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "object.h"

void initTable(Table * t){
    t->capacity =0;
    t->count=0;
    t->entries=NULL;
}
void freeTable(Table *t) {
    FREE_ARRAY(Entry, t->entries, t->capacity);
    initTable(t);
}
bool getTable(Table *t, ObjString *k, Value *out_v) {
    // false si no esta
    if ( t->count == 0) return false;
    Entry* e = findEntry(t->entries, t->capacity, k);
    if (e->key == NULL) return false;
    *out_v = e->value;
    return true;
}
bool deleteTable(Table *t, ObjString *k) {
    if (t->count == 0) return false;
    Entry* e = findEntry(t->entries, t->capacity, k);
    if (e->key == NULL) return false;
    e->key = NULL;
    e->value = VAL_BOOL(true);
    return true;
}
bool setTable(Table * t, ObjString *k, Value v) {
    // true si es nueva, false si no
    if (t->count + 1 > t->capacity * MAX_LOAD_FACTOR) {
        int nueva_cap = GROW_CAPACITY(t->capacity);
        adjustCapacty(t, nueva_cap);
    }
    Entry* looked = findEntry(t->entries, t->capacity, k);
    
    bool isNew = looked->key == NULL;
    if ( isNew && IS_NIL(looked->value)) t->count++;

    looked->key = k;
    looked->value = v;
    return isNew;
}
Entry* findEntry(Entry * e, int n, ObjString * k) {
    uint32_t idx = k->hash % n;
    Entry* tombstone = NULL;
    for (;;) {
        Entry* e_i = &e[idx];
        if (e_i->key == NULL) {
            if (IS_NIL(e_i->value))
                return tombstone == NULL ? e_i : tombstone;
            else if (tombstone == NULL) 
                tombstone = e_i;
        }else if (k == e_i->key) 
            return e_i;
        idx = (idx+1)%n  ;      
    }
}
void adjustCapacty(Table* t, int nueva_cap){
    Entry* n_e = ALLOCATE(Entry, nueva_cap);
    for(int i = 0; i < nueva_cap; i++){
        n_e[i].key = NULL;
        n_e[i].value = VAL_NIL();
    }
    t->count = 0;
    for (int i = 0; i < t->capacity; i++) {
        Entry* e_i = &t->entries[i];
        if (e_i->key == NULL) continue;
        Entry* n_e_i = findEntry(n_e, nueva_cap, e_i->key);
        n_e_i->key = e_i->key;
        n_e_i->value = e_i->value;
        t->count++;
    }
    FREE_ARRAY(Entry, t->entries, t->capacity);
    t->entries = n_e;
    t->capacity = nueva_cap;
}
uint16_t hashString(const char* s, int n) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < n; i++) {
        hash ^= *(s+i);
        hash *= 16777619;;
    }
    return hash;
}

void tableAddAll(Table * from, Table * to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* e = &from->entries[i];
        if (e->key != NULL) 
            setTable(to, e->key, e->value);
    }
}
// INTERNING STRINGS
ObjString* findStringTable(Table* t, const char* s, int n, uint32_t h) {
    if (t->count==0) return NULL;

    uint32_t idx = h % t->capacity;
    for(;;) {
        Entry* e = &t->entries[idx];
        if (e->key != NULL) {
            if (e->key->length == n && e->key->hash ==h && memcmp(e->key->chars, s, n) == 0)
                return e->key;
        }else if (IS_NIL(e->value)){
            return NULL;
        }

        idx = (idx+1)%t->capacity;
    }
}