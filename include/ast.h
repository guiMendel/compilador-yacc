#ifndef AST_H
#define AST_H

#include "list.h"
#include <stdint.h>

typedef enum {
  BINOP_ADD,
  BINOP_SUB,
  BINOP_MUL,
  BINOP_DIV,
  BINOP_EQ,
  BINOP_NEQ,
  BINOP_LT,
  BINOP_GT,
  BINOP_AND,
  BINOP_OR,
} BinOp;

typedef enum {
  UNOP_NEG,
  UNOP_NOT,
} UnOp;

typedef enum {
  AST_BLOCK,
  AST_RETURN,
  AST_NUMBER,
  AST_BINOP,
  AST_UNOP,
  AST_IDENT,
  AST_ASSIGN,
  AST_IF,
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

    struct {
      struct AstNode *left;
      struct AstNode *right;
      BinOp op;
    } as_binop;

    struct {
      struct AstNode *expr;
      UnOp op;
    } as_unop;

    struct {
      char *name;
    } as_ident;

    struct {
      char *name;
      struct AstNode *expr;
    } as_assign;

    struct {
      struct AstNode *cond;
      struct AstNode *then;
      struct AstNode *els;
    } as_if;
  };
} AstNode;

void ast_block_init(AstNode *node);
void ast_number_init(AstNode *node, int32_t value);
void ast_return_init(AstNode *node, AstNode *expr);
void ast_binop_init(AstNode *node, BinOp op, AstNode *left, AstNode *right);
void ast_unop_init(AstNode *node, UnOp op, AstNode *expr);
void ast_ident_init(AstNode *node, char *name);
void ast_assign_init(AstNode *node, char *name, AstNode *expr);
void ast_if_init(AstNode *node, AstNode *cond, AstNode *then, AstNode *els);

typedef struct {
  char *source_name;
  int line_defined;
  int num_params;
  uint8_t is_vararg;
  int max_stack;

  list kstr;
  list knum;
  list kfunc;

  AstNode code; // pointer to a block node
} Function;

void function_init(Function *f, char *source_name);

#endif // AST_H
