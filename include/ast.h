#ifndef AST_H
#define AST_H

#include "list.h"
#include <stdint.h>

typedef struct Function Function;

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
  AST_WHILE,
  AST_CALL,
  AST_READ,
  AST_PRINT,
  AST_FUNCTION,
} NodeType;

typedef struct AstNode {
  NodeType type;
  union {
    struct {
      List stmts;
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
      int index;
      int is_local;
      int is_upvalue;
    } as_ident;

    struct {
      int index;
      int is_decl;
      struct AstNode *expr;
    } as_assign;

    struct {
      struct AstNode *cond;
      struct AstNode *then;
      struct AstNode *els;
    } as_if;

    struct {
      struct AstNode *cond;
      struct AstNode *body;
    } as_while;

    struct {
      int index;
      List *args;
      int is_local;
      int is_upvalue;
    } as_call;

    struct {
      int index;
    } as_read;

    struct {
      struct AstNode *expr;
    } as_print;

    struct {
      int name_index;
      int fn_index;
      Function *fn;
      List *args;
    } as_function;
  };
} AstNode;

/*
 * A function prototype.
 */
typedef struct Function {
  char *source_name;

  List params;
  List locals;

  int max_stack;
  int depth;

  /* from lua (always 0) */
  int line_defined;
  uint8_t is_vararg;

  List kstr;
  List knum;
  List kfunc;

  struct Function *parent;

  List upvalues;

  AstNode *code; // pointer to a block node
} Function;

AstNode *new_block_node();
AstNode *new_number_node(int32_t value);
AstNode *new_return_node(AstNode *expr);
AstNode *new_binop_node(BinOp op, AstNode *left, AstNode *right);
AstNode *new_unop_node(UnOp op, AstNode *expr);
AstNode *new_if_node(AstNode *cond, AstNode *then, AstNode *els);
AstNode *new_while_node(AstNode *cond, AstNode *body);

AstNode *new_ident_node(char *name, Function *fn);
AstNode *new_assign_node(char *name, AstNode *expr, int is_decl, Function *fn);
AstNode *new_call_node(char *name, List *args, Function *fn);
AstNode *new_read_node(char *name, Function *fn);
AstNode *new_print_node(AstNode *expr);

AstNode *new_function_node(char *name, List *params, Function *fn);

void declareVar(char *name, Function *fn);

void function_init(Function *f, char *source_name);
Function *new_function(char *source_name, Function *parent);

#endif // AST_H
