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
