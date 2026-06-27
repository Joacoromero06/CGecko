#include "scanner.h"
#include <stdbool.h>
#include <string.h>


//===FORWARD DECLARATIONS===
static char advance();
static Token number();
static Token string();
static bool isAtEnd();
static bool match(char);

//===GLOBALES===
Scanner scanner;

void initScanner(const char *source) {
  scanner.current = scanner.start = (char*) source;
  scanner.line = 1;
}
Token scanToken(){
  skipWhiteSpaces();
  

  scanner.start = scanner.current;
  if (isAtEnd()) return createToken(TOKEN_EOF);
  
  char c = advance();

  if (isDigit(c)) return number();
  if (isAlpha(c)) return id();
  
  
  switch (c) {
    case '.': return createToken(TOKEN_PUNTO);
    case ',': return createToken(TOKEN_COMA);
    case ';': return createToken(TOKEN_PUNTOCOMA);
    case '&': return createToken(TOKEN_AMPERSAN);
    case '(': return createToken(TOKEN_LPAREN);
    case ')': return createToken(TOKEN_RPAREN);
    case '{': return createToken(TOKEN_LBRACE);
    case '}': return createToken(TOKEN_RBRACE);
    case '[': return createToken(TOKEN_LCORCH);
    case ']': return createToken(TOKEN_RCORCH);
    case '+': return createToken(TOKEN_SUMA);
    case '-': return createToken(TOKEN_RESTA);
    case '*': return createToken(TOKEN_MULTIPLICACION);
    
    case '/': return createToken(TOKEN_DIVISION);

    case '=': return createToken(match('=') ? TOKEN_IGUALES : TOKEN_IGUAL);
    case '!': return createToken(match('=') ? TOKEN_DISTINTOS : TOKEN_BANG);
    case '<': return createToken(match('=') ? TOKEN_MENOR_IGUAL : TOKEN_MENOR);
    case '>': return createToken(match('=') ? TOKEN_MAYOR_IGUAL : TOKEN_MAYOR);

    case '"': return string();
  }
  return errorToken("[ERROR] caracter no valido.");
}

Token createToken(TokenType type){
  Token t;
  t.type = type;
  t.length = (int) (scanner.current - scanner.start);
  t.line = scanner.line;
  t.start = scanner.start;
  return t;
}
Token errorToken(char *msg){
  Token t;
  t.type = TOKEN_ERROR;
  t.line = scanner.line;
  t.start = msg;
  t.length = (int) strlen(msg);
  return t;
}

static bool isAlpha(char c){
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_';
}
static bool isDigit(char c){
  return ('0' <= c && c <= '9');
}
static bool isAtEnd(){
  return *scanner.current == '\0';
}
static char peek(){
  return *scanner.current;
}
static char peekNext(){
  if (isAtEnd()) return '\0';
  return *(scanner.current+1);

}
static char advance(){
  scanner.current ++;
  return *(scanner.current-1); // mismo que scanner.curren[-1]?
}
static bool match(char c){
  if (isAtEnd()) return false;
  if (peek() != c) return false;
  scanner.current++;
  return true;
}
static void skipWhiteSpaces(){
  for (;;){
    char c = peek();
    switch (c) {
    case ' ': 
    case '\t':
    case '\r':
      advance();
      break;
    case '\n':
      scanner.line++;
      advance();
      break;
    case '/':
      if (peekNext() == '/') {
        while (peek() != '\n' && !isAtEnd()) advance();
      } else {
        return;
      }
      break;
    default: 
      return;
    }
  }
}
static Token number(){
  while (isDigit(peek())) advance();
  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) advance();
  }
  return createToken(TOKEN_NUMERO);
}
static Token string(){
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') scanner.line++;
    advance();
  }
  if (advance() != '"') return errorToken("[ERROR] string literal sin cerrar");
  return createToken(TOKEN_STRING);
}
static Token id(){
  while (isDigit(peek()) || isAlpha(peek())) advance();
  return checkKeyWord();
}
static Token checkKeyWord(){
  switch (*scanner.start) {
  case 'a': return checkRest(1, 2, "nd", TOKEN_AND);
  case 'c': return checkRest(1, 4, "lass", TOKEN_CLASS);
  case 'i': return checkRest(1, 1, "f", TOKEN_IF);
  case 'p': return checkRest(1, 4, "rint",TOKEN_PRINT);
  case 'o': return checkRest(1, 1, "r", TOKEN_OR);  
  case 'n': return checkRest(1, 2, "il", TOKEN_NIL);
  case 's': return checkRest(1, 4, "uper", TOKEN_SUPER);  
  case 'm': return checkRest(1, 2, "ut", TOKEN_MUT);
  case 'v': return checkRest(1, 2, "ar", TOKEN_VAR);
  case 'w': return checkRest(1, 4, "hile", TOKEN_WHILE);
  case 'l': return checkRest(1, 2, "et", TOKEN_LET);
  case 'r': 
    if (*(scanner.start+1) == 'e'){
      switch (*(scanner.start+2)) {
      case 'f': return createToken(TOKEN_REF);
      case 't': return checkRest(3, 3, "urn", TOKEN_RETURN);
      default: return createToken(TOKEN_IDENTIFIER);
      }
    } else return createToken(TOKEN_IDENTIFIER);
  case 'f': 
    switch (*(scanner.start+1)) {
    case 'a': return checkRest(2, 3, "lse", TOKEN_FALSE);
    case 'o': return checkRest(2, 1, "r", TOKEN_FOR);
    case 'u': return checkRest(2, 1, "n", TOKEN_FUN);
    default: return createToken(TOKEN_IDENTIFIER);
    }
  case 't': 
    switch (*(scanner.start+1)) {
    case 'h': 
        switch(*(scanner.start+2)) {
        case 'i': return checkRest(3,1, "s", TOKEN_THIS);
        case 'e': return checkRest(3,1, "n", TOKEN_THEN);
        default: return createToken(TOKEN_IDENTIFIER);
        }
    case 'r': return checkRest(2, 2, "ue", TOKEN_TRUE);
    default: return createToken(TOKEN_IDENTIFIER);
    }
  case 'e':
    switch (*(scanner.start+1)) {
    case 'l': return checkRest(2, 2, "se", TOKEN_ELSE);
    case 'n': return checkRest(2, 1, "d", TOKEN_END);
    default: return createToken(TOKEN_IDENTIFIER);
    }
  default: return createToken(TOKEN_IDENTIFIER);
  }
  return createToken(TOKEN_IDENTIFIER);
}
static Token checkRest(int start, int lenght, const char* rest, TokenType type){
  if (start + lenght == scanner.current - scanner.start &&
    memcmp(scanner.start + start, rest, lenght) == 0 ){
    return createToken(type);
  }
  return createToken(TOKEN_IDENTIFIER);
}
