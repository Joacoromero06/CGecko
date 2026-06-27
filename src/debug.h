#include "chunk.h"

#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

void disambleChunk(Chunk*, const char*);
int disambleInstruction(Chunk *, int);
int simpleInstruction(const char*, int);
int constantInstruction(const char*, Chunk*, int);
int indexInstruction(const char*, Chunk*, int);
int jumpInstruction(const char*, int, Chunk*, int);

