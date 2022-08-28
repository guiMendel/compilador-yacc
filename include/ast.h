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
  AST_WHILE,
  AST_CALL,
  AST_READ,
  AST_FUNCTION,

  AST_ARRAY,
  AST_ARRAY_ACCESS,
  AST_ARRAY_ASSIGN,

  AST_EACH,
  AST_FOR,
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
    } as_ident;

    struct {
      int index;
      int is_local;
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
    } as_call;

    struct {
      int index;
    } as_read;

    struct {
      int name_index;
      int fn_index;
      List *args;
    } as_function;

    struct {
      List *args;
    } as_array;

    struct {
      struct AstNode *array;
      struct AstNode *index;
    } as_array_access;

    struct {
      int array_index;
      struct AstNode *index;
      struct AstNode *expr;
      int is_local;
    } as_array_assign;

    struct {
      struct AstNode *expr;
      struct AstNode *body;
    } as_each;

    struct {
      struct AstNode *init;
      struct AstNode *limit;
      struct AstNode *step;
      struct AstNode *body;
    } as_for;
  };
} AstNode;

/*
 * A function prototype.
 */
typedef struct Function {
  char *source_name;

  List locals;
  List params;
  int max_stack;

  /* from lua (always 0) */
  int line_defined;
  uint8_t is_vararg;

  List kstr;
  List knum;
  List kfunc;

  struct Function *parent;

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
AstNode *new_assign_node(char *name, AstNode *expr, Function *fn);
AstNode *new_call_node(char *name, List *args, Function *fn);
AstNode *new_read_node(char *name, Function *fn);

AstNode *new_function_node(char *name, List *params, Function *fn);

AstNode *new_array_node(List *args);
AstNode *new_array_access_node(AstNode *array, AstNode *index);
AstNode *new_array_assign_node(char* name, AstNode *index, AstNode *expr, Function *fn);

AstNode *new_each_node(char *ident, AstNode *expr, AstNode *body, Function *fn);
AstNode *new_for_node(char *ident, AstNode *init, AstNode *limit, AstNode *body);

int declareVar(char *name, Function *fn);

void function_init(Function *f, char *source_name);
Function *new_function(char *source_name, Function *parent);

#endif // AST_H
