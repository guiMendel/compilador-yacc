#ifndef AST_H
#define AST_H

#include <stdint.h>
#include "list.h"

typedef enum {
  AST_BLOCK,
  AST_RETURN,
  AST_NUMBER,
} NodeType;

typedef struct AstNode {
  NodeType type;
  union {
    struct {
      list stmts;
    } as_block;

    struct {
      int32_t value;
    } as_number;

    struct {
      struct AstNode *expr;
    } as_ret;
  };
} AstNode;

void ast_block_init(AstNode *node);
void ast_number_init(AstNode *node, int32_t value);
void ast_return_init(AstNode *node, AstNode *expr);

typedef struct {
  char* source_name;
  int line_defined;
  int num_params;
  uint8_t is_vararg;
  int max_stack;

  list kstr;
  list knum;
  list kfunc;

  AstNode code; // pointer to a block node
} Function;

void function_init(Function* f, char* source_name);


#endif // AST_H
