#ifndef scanner_h
#define scanner_h
#include <stdbool.h>

typedef enum{
    // UN CARACTER
    TOKEN_SUMA,      TOKEN_RESTA,  TOKEN_MULTIPLICACION, TOKEN_DIVISION,
    TOKEN_LPAREN,    TOKEN_RPAREN, TOKEN_LBRACE,         TOKEN_RBRACE,  TOKEN_LCORCH, TOKEN_RCORCH,
    TOKEN_PUNTOCOMA, TOKEN_COMA,   TOKEN_PUNTO,          TOKEN_AMPERSAN,

    // UNO O DOS CARACTERES
    TOKEN_MAYOR, TOKEN_MENOR, TOKEN_MAYOR_IGUAL, TOKEN_MENOR_IGUAL,
    TOKEN_BANG,  TOKEN_IGUAL, TOKEN_IGUALES,     TOKEN_DISTINTOS,

    // PALABRAS RESERVADAS
    TOKEN_FOR,   TOKEN_WHILE,  TOKEN_IF,    TOKEN_ELSE, TOKEN_THEN,
    TOKEN_FUN,   TOKEN_RETURN, TOKEN_END, 
    TOKEN_LET,   TOKEN_VAR,    TOKEN_REF,   TOKEN_MUT,
    TOKEN_CLASS, TOKEN_THIS,   TOKEN_SUPER, TOKEN_NIL,
    TOKEN_AND,   TOKEN_OR,     TOKEN_TRUE,  TOKEN_FALSE,
    TOKEN_PRINT, 

    // LITERALES SEMANTICOS
    TOKEN_STRING, TOKEN_NUMERO, TOKEN_IDENTIFIER,

    TOKEN_EOF,
    TOKEN_ERROR
}TokenType;
typedef struct{
    TokenType type;
    char* start;
    int length;
    int line;
}Token;
Token createToken(TokenType);

typedef struct{
    char* start;
    char* current;
    int line;
}Scanner;

void initScanner(const char*);
Token scanToken();
Token errorToken(char*);

//static bool isAtEnd();
//static char advance();
//static char peek();
//static char peekNext();
//static bool match(char);

static void skipWhiteSpaces();
static bool isDigit(char);
static bool isAlpha(char);
//static Token number();
//static Token string(); 
static Token id();
static Token checkKeyWord();
static Token checkRest(int, int, const char*, TokenType);
#endif
