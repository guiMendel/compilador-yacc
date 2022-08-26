#ifndef AST_H
#define AST_H

#include <stdint.h>
#include "list.h"

typedef enum {
  AST_BLOCK,
} NodeType;

typedef struct {
  NodeType type;
  union {
    struct {
      list stmts;
    } block;
  };
} AstNode;

void ast_node_init(AstNode *node, NodeType type);

typedef struct {
  char* source_name;
  int line_defined;
  int num_params;
  uint8_t is_vararg;
  int max_stack;

  list kstr;
  list knum;
  list kfunc;

  AstNode block; // pointer to a block node
} Function;

void function_init(Function* f, char* source_name);


#endif // AST_H
