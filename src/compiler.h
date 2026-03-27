#ifndef compiler_h
#define compiler_h


#include "scanner.h"
#include "chunk.h"
#include "value.h"
#include <stdint.h>
//#include "vm.h"


bool compile(const char*, Chunk*);

typedef struct{
    Token previous;
    Token current;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum{
    PRECEDENCIA_NONE,
    PRECEDENCIA_ASIGNACION, // =
//PRECEDENCIA_TERNARIO,   // ?:
    PRECEDENCIA_OR,         // or
    PRECEDENCIA_AND,        // and
    PRECEDENCIA_IGUALDADES, // == !=
    PRECEDENCIA_COMPARACIONES, // < <= > >=
    PRECEDENCIA_TERMINO,    // + -
    PRECEDENCIA_FACTOR,     // * /
    PRECEDENCIA_UNARIO,     // - !
    PRECEDENCIA_CALL,       // . ()
//PRECEDENCIA_AGRUPACION, // ()
    PRECEDENCIA_LITERAL,    // n s b id
} Precedence;

typedef void (*ParseFn)(bool);
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

#define UINT8_COUNT (UINT8_MAX + 1)
typedef struct {
    Token name;
    int depth;
} Local;
typedef struct {
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
} Compiler;
void initCompiler(Compiler*);
void freeCompiler(Compiler*);
static void beginScope();
static void endScope();

// forwarded:: static void advance();
static void errorAtCurrent(const char*);
static void errorAtPrevious(const char*);
static void errorAt(Token*, const char*);
static void consume(TokenType, const char*);

//static bool isAtEnd();
//static bool match(TokenType);
//static bool check(TokenType);

static void declaration();
static void statement();
static void printStatement();
static void expressionStatement();

static void parsePrecedence(Precedence);
static void expression();

static void grouping(bool);
static void unary(bool);

static void binary(bool);
static ParseRule* getRule(TokenType);

static void literal(bool);
// forwarded:: static void number(bool);
// forwarded:: static void string(bool);

static void emitConstant(Value);
static uint8_t makeConstant(Value);


static Chunk* currentChunk();
static void emitByte(uint8_t);
static void endCompiler();
static void emitReturn();
static void emitBytes(uint8_t, uint8_t);

#endif