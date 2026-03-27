#include "vm.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h> 

static void repl();
static void script(const char *);
static char *readFile(const char *);

typedef enum { e_AL_ABRIR, e_SIN_MEMORIA, e_AL_LEER } TExit;

int main(int argc, const char *argv[]) {
  initVM();
  
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    script(argv[1]);
  }

  freeVM();
  return 0;
}

static void repl() {
  char line[1024];
  for (;;) {
    printf("> ");
    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }
    interpret(line);
  }
}
static void script(const char *fn) {
  char *source = readFile(fn);
  InterpretResult result = interpret(source);
  free(source);

  switch (result) {
  case COMPILE_ERROR:
    exit(COMPILE_ERROR);
    break;
  case RUNTIME_ERROR:
    exit(RUNTIME_ERROR);
    break;
  default:
    break;
  }
}
static char *readFile(const char *fn) {
  FILE *f = fopen(fn, "rb");
  if (!f) {
    fprintf(stderr, "[ERROR] no se pudo abrir \"%s\".\n", fn);
    exit(e_AL_ABRIR);
  }

  fseek(f, 0L, SEEK_END);
  size_t size = ftell(f);
  rewind(f);

  char *buffer = (char *)malloc(size + 1);
  if (!buffer) {
    fprintf(stderr, "[ERROR] sin memoria, archivo muy grande.\n");
    exit(e_SIN_MEMORIA);
  }

  size_t bytesRead = fread(buffer, sizeof(char), size, f);
  if (bytesRead < size){
    fprintf(stderr, "[ERROR] no se pudo leer el archivo \"%s\".\n", fn);
    exit(e_AL_LEER);
  }
  buffer[bytesRead] = '\0';

  fclose(f);
  return buffer;
}