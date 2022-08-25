#ifndef AST_H
#define AST_H

#include <stdint.h>
#include "list.h"

typedef struct {
  char* source_name;
  int line_defined;
  int num_params;
  uint8_t is_vararg;
  int max_stack;

  list kstr;
  list knum;
  list kfunc;

  void* code; // pointer to a block node
} Function;

void function_init(Function* f, char* source_name);

#endif // AST_H
