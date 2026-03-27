#include "compiler.h"
#include "chunk.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "value.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===FORWARD DECLARATIONS===
static void advance();
static void number(bool);
static void string(bool); 
static void variable(bool);
static bool match(TokenType);

// ===GLOBALES===
Parser parser;
Chunk *compilingChunk;
Compiler* current = NULL;
ParseRule rules[] = {
  
    [TOKEN_MULTIPLICACION] = {NULL, binary, PRECEDENCIA_FACTOR},
    [TOKEN_SUMA]     = {NULL,binary,PRECEDENCIA_TERMINO,},
    [TOKEN_RESTA]    = {unary, binary, PRECEDENCIA_TERMINO},
    [TOKEN_DIVISION] = {NULL, binary, PRECEDENCIA_FACTOR},

    [TOKEN_LPAREN] = {grouping, NULL, PRECEDENCIA_LITERAL},
    [TOKEN_RPAREN] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_LBRACE] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_RBRACE] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_LCORCH] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_RCORCH] = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_PUNTO]     = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_PUNTOCOMA] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_COMA]      = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_AMPERSAN]  = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_MAYOR]       = {NULL, binary, PRECEDENCIA_COMPARACIONES},
    [TOKEN_MAYOR_IGUAL] = {NULL, binary, PRECEDENCIA_COMPARACIONES},
    [TOKEN_MENOR]       = {NULL, binary, PRECEDENCIA_COMPARACIONES},
    [TOKEN_MENOR_IGUAL] = {NULL, binary, PRECEDENCIA_COMPARACIONES},
    [TOKEN_IGUAL]       = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_IGUALES]     = {NULL, binary, PRECEDENCIA_IGUALDADES},
    [TOKEN_BANG]        = {unary, NULL, PRECEDENCIA_NONE},
    [TOKEN_DISTINTOS]   = {NULL, binary, PRECEDENCIA_IGUALDADES},

    [TOKEN_TRUE]  = {literal, NULL, PRECEDENCIA_NONE},
    [TOKEN_FALSE] = {literal, NULL, PRECEDENCIA_NONE},
    [TOKEN_AND]   = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_OR]    = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_IF]    = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_ELSE]  = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_FOR]   = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_FUN]    = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_END]    = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_VAR] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_REF] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_MUT] = {NULL, NULL, PRECEDENCIA_NONE},

    [TOKEN_CLASS] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_THIS]  = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_NIL]   = {literal, NULL, PRECEDENCIA_NONE},

    [TOKEN_STRING]     = {string, NULL, PRECEDENCIA_NONE},
    [TOKEN_NUMERO]     = {number, NULL, PRECEDENCIA_NONE},
    [TOKEN_IDENTIFIER] = {variable, NULL, PRECEDENCIA_NONE},

    [TOKEN_EOF]   = {NULL, NULL, PRECEDENCIA_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PRECEDENCIA_NONE},

};

bool compile(const char *source, Chunk *chunk) {
  initScanner(source);

  Compiler compiler;
  initCompiler(&compiler);

  compilingChunk = chunk;
  parser.panicMode = parser.hadError = false;

  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_EOF, "Esperaba fin de la expresion.");
  endCompiler();
  return !parser.hadError;
}

/*==================================*/
/*        CONNECTING SCANNER        */
/*==================================*/
static void advance() {
  parser.previous = parser.current;
  for (;;) {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR)
      break;
    errorAtCurrent(parser.current.start);
  }
}
static bool check(TokenType type) {
  return type == parser.current.type;
}
static bool match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}
static void errorAtCurrent(const char *msg) { errorAt(&parser.current, msg); }
static void errorAtPrevious(const char *msg) { errorAt(&parser.previous, msg); }
static void errorAt(Token *t, const char *msg) {
  if (parser.panicMode)
    return;
  parser.panicMode = true;

  fprintf(stderr, "[ERROR] linea %2d, ", t->line);
  if (t->type == TOKEN_EOF)
    fprintf(stderr, "al final.\n");
  if (t->type == TOKEN_ERROR) {
    fprintf(stderr, "token error \n");
  } else
    fprintf(stderr, "en \"%.*s\".\n", t->length-1, t->start);
  parser.hadError = true;
}
static void consume(TokenType type, const char *msg) {
  if (parser.current.type == type) {
    advance();
    return;
  }
  errorAtCurrent(msg);
}

/*==================================*/
/*            COMPILING             */
/*==================================*/
void initCompiler(Compiler* compiler) {
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  current = compiler;
}
void freeCompiler(Compiler * compiler) {
  initCompiler(compiler);
}
static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte, parser.previous.line);
}
static Chunk *currentChunk() { return compilingChunk; }
static void endCompiler() {
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disambleChunk(currentChunk(), "BYTECODE de source");
  }
#endif
  emitReturn();
}
static void emitReturn() { emitByte(OP_RETURN); }
static void emitBytes(uint8_t b1, uint8_t b2) {
  emitByte(b1);
  emitByte(b2);
}

/*==================================*/
/*            STATEMENTS            */
/*==================================*/
static void synchronize() {
   parser.panicMode = false;
  advance();
  while (parser.current.type != TOKEN_EOF) {
    if (parser.previous.type == TOKEN_PUNTOCOMA) return;
    switch (parser.current.type) {
    case TOKEN_FOR:
    case TOKEN_CLASS:
    case TOKEN_FUN:
    case TOKEN_IF:
    case TOKEN_WHILE:
    case TOKEN_RETURN:
    case TOKEN_PRINT:
    case TOKEN_VAR: return;
    default:;
    }   
    advance();
  }
}
static void printStatement() {
  expression();
  consume(TOKEN_PUNTOCOMA, "Esperaba ';' al final de la sentencia print.");
  emitByte(OP_PRINT);
}
static void expressionStatement() {
  expression();
  consume(TOKEN_PUNTOCOMA, "Esperaba ';' al final de la sentencia.");
  emitByte(OP_POP);
}
static void beginScope() {
  current->scopeDepth++;
}
static void endScope() {
  current->scopeDepth--;
  
  while (current->localCount > 0 && 
    current->locals[current->localCount-1].depth > current->scopeDepth) {

    emitByte(OP_POP);
    current->localCount--;
  }
}
static void block() {
  while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF)) {
    declaration();
  }
  consume(TOKEN_RBRACE, "Esperaba '}' para finalizar el bloque.");
}
static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  }
  else if (match(TOKEN_LBRACE)) {
    beginScope();
    block();
    endScope();
  }
  else {
    expressionStatement();

  }
}

/*==================================*/
/*           DECLARATIONS           */
/*==================================*/
static bool idEquals(Token* a, Token* b){ 
  if (a->length != b->length) return false;
  return memcmp(a->start, b->start, a->length)==0;
}
static void addLocal(Token name) {
  if ( current->localCount == UINT8_COUNT) {
    errorAtPrevious("Excedio el limite de variables en una funcion.");
  }

  Local* local = &current->locals[current->localCount++];
  local->name = name;
  local->depth = -1; // initialized no ready for use
}
static void declareVar() {
  // no declarar en compile time globals
  if (current->scopeDepth == 0) return;

  Token *name = &parser.previous;

  for (int i = current->localCount-1; i >= 0; i--) {
    Local* local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scopeDepth) 
      break;
    if ( idEquals(&local->name, name) )
      errorAtPrevious("Ya existe una variable con este nombre en el bloque.");
    
  }
  addLocal(*name);
}
static uint8_t identifierConstant(Token* t) {
  return makeConstant(VAL_OBJ(copyString(t->start, t->length)));
}
static uint8_t parseVar(const char* errmsg) {
  consume(TOKEN_IDENTIFIER, errmsg);

  declareVar();
  if (current->scopeDepth > 0) return 0; // no agregar si es una local

  return identifierConstant(&parser.previous);
}
static void markInitialized() {
  current->locals[current->localCount-1].depth = current->scopeDepth;
}
static void defineVar(uint8_t global_idx) {
  markInitialized();
  // no compilar ninguna instruccion para locals
  /* el rvalue esta en el top del stack al ejecutar, se considerara un local place, no es necesario
  una operacion OP_DECL_LOCAL -> eficiencia. No necesitamos ver la instruccion, la constante, el ObjString
  realizar el setTable.*/
  if (current->scopeDepth > 0) return; 

  emitBytes(OP_DECL_GLOBAL, global_idx);
}
static void varDeclaration() {
  uint8_t global_var = parseVar("Esperaba nombre de variable.");
  if (match(TOKEN_IGUAL)){
    expression();
  }else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_PUNTOCOMA, "Esperaba ';' al final de la declaracion de variable.");
  defineVar(global_var);
}
static void declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  }
  else {
    statement();
  }
  if (parser.panicMode) synchronize();
}

/*==================================*/
/*           EXPRESSIONS            */
/*==================================*/
static void expression() { parsePrecedence(PRECEDENCIA_ASIGNACION); }
static void parsePrecedence(Precedence precedencia) {
  advance();
  ParseFn prefix = getRule(parser.previous.type)->prefix;
  if (prefix == NULL) {
    errorAtPrevious("Esperaba una expresion.");
    return;
  }
  bool canAssign = precedencia <= PRECEDENCIA_ASIGNACION;
  prefix(canAssign);

  while (getRule(parser.current.type)->precedence >= precedencia) {
    advance();
    ParseFn infix = getRule(parser.previous.type)->infix;
    infix(canAssign);
  } 
  if (canAssign && check(TOKEN_IGUAL)) { // el token igual deberia consumirse en namedVariable
    errorAtCurrent("No es un valor asignable.");
  }
}
static ParseRule *getRule(TokenType op) { return &rules[op]; }
static void binary(bool canAssign) {
  TokenType op = parser.previous.type;

  ParseRule *rule = getRule(op);
  parsePrecedence((Precedence)(rule->precedence + 1));

  switch (op) {
  case TOKEN_MAYOR: emitByte(OP_MAYOR); break;
  case TOKEN_MENOR: emitByte(OP_MENOR); break;
  case TOKEN_MAYOR_IGUAL: emitBytes(OP_MENOR, OP_NEGAR); break;
  case TOKEN_MENOR_IGUAL: emitBytes(OP_MAYOR, OP_NEGAR); break;
  case TOKEN_IGUALES: emitByte(OP_IGUALES); break;
  case TOKEN_DISTINTOS: emitBytes(OP_IGUALES, OP_NEGAR); break;
  
  case TOKEN_SUMA: emitByte(OP_SUMAR); break;
  case TOKEN_RESTA: emitByte(OP_RESTAR); break;
  case TOKEN_MULTIPLICACION: emitByte(OP_MULTIPLICAR); break;
  case TOKEN_DIVISION: emitByte(OP_DIVIDIR); break;
  default: return;
  }
}
static void grouping(bool canAssign) {
  expression();
  consume(TOKEN_RPAREN, "Esperaba ')' para cerrar la agrupacion. ");
}
static void unary(bool canAssign) {
  TokenType type = parser.previous.type;
  parsePrecedence(PRECEDENCIA_UNARIO);
  switch (type) {
  case TOKEN_RESTA: emitByte(OP_MENOS_UNARIO); break;
  case TOKEN_BANG: emitByte(OP_NEGAR); break;
  default: return;
  }
}
static void number(bool canAssign) {
  double num = strtod(parser.previous.start, NULL);
  emitConstant(VAL_NUM(num));
}
static void string(bool canAssign) {
  emitConstant(VAL_OBJ(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}
static void literal(bool canAssign) {
  switch (parser.previous.type) {
  case TOKEN_TRUE: emitByte(OP_TRUE); break;
  case TOKEN_FALSE: emitByte(OP_FALSE); break;
  case TOKEN_NIL: emitByte(OP_NIL); break;
  default: return;
  }
}
static int resolveLocal(Compiler* compiler, Token* name) {
  /*This index will works for stack of vm because locals and stack are equivalent before stmt execution*/
  for (int i = compiler->localCount-1; i>=0; i--) {
    Local* local = &compiler->locals[i];
    if (idEquals(&local->name, name)) {
      if (local->depth == -1)
        errorAtPrevious("No se puede definir una variable con sigo misma.");
      return i;
    }
  }
  return -1;
}
static void namedVariable(Token name, bool canAssign) {
  uint8_t op_set, op_get;
  int local_index = resolveLocal(current, &name);
  if (local_index != -1) {
    op_set = OP_SET_LOCAL;
    op_get = OP_GET_LOCAL;
  }
  else {
    local_index = identifierConstant(&name);
    op_set = OP_SET_GLOBAL;
    op_get = OP_GET_GLOBAL;
  }

  if (canAssign && match(TOKEN_IGUAL)) {
    expression();
    emitBytes(op_set, (uint8_t)local_index);
  }
  else {
    emitBytes(op_get, (uint8_t)local_index);
  }
}
static void variable(bool canAssign) {
  namedVariable(parser.previous, canAssign);
}

static void emitConstant(Value v) { emitBytes(OP_CONST, makeConstant(v)); }
static uint8_t makeConstant(Value v) {
  int constant = addConstant(currentChunk(), v);
  if (constant > UINT8_MAX) {
    errorAtPrevious("Demasiadas constantes en un mismo chunk.");
    return 0;
  }
  return (uint8_t)constant;
}