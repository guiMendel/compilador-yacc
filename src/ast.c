#include "ast.h"

#include <stdlib.h>

AstNode *new_block_node() {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_BLOCK;
  list_init(&node->as_block.stmts);
  return node;
}

AstNode *new_number_node(int32_t value) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_NUMBER;
  node->as_number.value = value;
  return node;
}

AstNode *new_return_node(AstNode *expr) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_RETURN;
  node->as_ret.expr = expr;
  return node;
}

AstNode *new_binop_node(BinOp op, AstNode *left, AstNode *right) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_BINOP;
  node->as_binop.op = op;
  node->as_binop.left = left;
  node->as_binop.right = right;
  return node;
}

AstNode *new_unop_node(UnOp op, AstNode *expr) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_UNOP;
  node->as_unop.op = op;
  node->as_unop.expr = expr;
  return node;
}

AstNode *new_ident_node(char *name) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_IDENT;
  node->as_ident.name = name;
  return node;
}

AstNode *new_assign_node(char *name, AstNode *expr) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_ASSIGN;
  node->as_assign.name = name;
  node->as_assign.expr = expr;
  return node;
}

AstNode *new_if_node(AstNode *cond, AstNode *then, AstNode *els) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_IF;
  node->as_if.cond = cond;
  node->as_if.then = then;
  node->as_if.els = els;
  return node;
}

AstNode *new_while_node(AstNode *cond, AstNode *body) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_WHILE;
  node->as_while.cond = cond;
  node->as_while.body = body;
  return node;
}

void function_init(Function *f, char *source_name) {
  f->source_name = source_name;
  f->line_defined = 0;
  f->num_params = 0;
  f->is_vararg = 0;
  f->max_stack = 0;

  list_init(&f->kstr);
  list_init(&f->knum);
  list_init(&f->kfunc);
}
