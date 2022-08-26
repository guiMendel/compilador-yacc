#include "ast.h"
#include "list.h"

#include <stdlib.h>

void ast_block_init(AstNode *node) {
  node->type = AST_BLOCK;
  list_init(&node->as_block.stmts);
}

void ast_number_init(AstNode *node, int32_t value) {
  node->type = AST_NUMBER;
  node->as_number.value = value;
}

void ast_return_init(AstNode *node, AstNode *expr) {
  node->type = AST_RETURN;
  node->as_ret.expr = expr;
}

void ast_binop_init(AstNode *node, BinOp op, AstNode *left, AstNode *right) {
  node->type = AST_BINOP;
  node->as_binop.left = left;
  node->as_binop.right = right;
  node->as_binop.op = op;
}

void ast_unop_init(AstNode *node, UnOp op, AstNode *expr) {
  node->type = AST_UNOP;
  node->as_unop.expr = expr;
  node->as_unop.op = op;
}

void ast_ident_init(AstNode *node, char *name) {
  node->type = AST_IDENT;
  node->as_ident.name = name;
}

void ast_assign_init(AstNode *node, char *name, AstNode *expr) {
  node->type = AST_ASSIGN;
  node->as_assign.name = name;
  node->as_assign.expr = expr;
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

  ast_block_init(&f->code);
}
