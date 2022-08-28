#include "ast.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int findGlobal(char *name, Function *fn) {
  Function *cur = fn;

  while (cur != NULL) {
    list_node *n = cur->kstr.head;
    int i = 0;

    while (n != NULL) {
      if (strcmp(name, (char *)n->data) == 0) {
        if (cur == fn)
          return i;
        else
          return declareVar(name, fn);
      }
      n = n->next;
      i++;
    }

    cur = cur->parent;
  }

  return -1;
}

static int findLocal(char *name, Function *fn) {
  list_node *n = fn->params.head;
  int i = 0;

  while (n != NULL) {
    if (strcmp(name, (char *)n->data) == 0) {
      return i;
    }
    n = n->next;
    i++;
  }

  return -1;
}

int declareVar(char *name, Function *fn) { 
  list_append(&fn->kstr, name); 
  return fn->kstr.size - 1;
}

AstNode *new_ident_node(char *name, Function *fn) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_IDENT;

  node->as_ident.is_local = 0;

  int index = findLocal(name, fn);
  if (index == -1) {
    index = findGlobal(name, fn);
    if (index == -1) {
      printf("Error: Unknown variable %s\n", name);
      exit(1);
    }
  } else {
    node->as_ident.is_local = 1;
  }

  node->as_ident.index = index;

  return node;
}

AstNode *new_assign_node(char *name, AstNode *expr, Function *fn) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_ASSIGN;

  node->as_assign.is_local = 0;

  int index = findLocal(name, fn);
  if (index == -1) {
    index = findGlobal(name, fn);
    if (index == -1) {
      printf("Error: Unknown variable %s\n", name);
      exit(1);
    }
  } else {
    node->as_assign.is_local = 1;
  }

  node->as_assign.index = index;

  node->as_assign.expr = expr;
  return node;
}

AstNode *new_call_node(char *name, List *args, Function *fn) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_CALL;
  node->as_call.index = findGlobal(name, fn);
  node->as_call.args = args;
  return node;
}

AstNode *new_read_node(char *name, Function *fn) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_READ;
  node->as_read.index = findGlobal(name, fn);
  return node;
}

AstNode *new_function_node(char *name, List *args, Function *fn) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_FUNCTION;

  // FIXME: this may not work for nested functions
  node->as_function.fn_index = fn->parent->kfunc.size - 1;

  node->as_function.name_index = findGlobal(name, fn->parent);
  node->as_function.args = args;
  return node;
}

AstNode *new_array_node(List *args) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_ARRAY;
  node->as_array.args = args;
  return node;
}

AstNode *new_array_access_node(AstNode *array, AstNode *index) {
  AstNode *node = malloc(sizeof(*node));
  node->type = AST_ARRAY_ACCESS;
  node->as_array_access.array = array;
  node->as_array_access.index = index;
  return node;
}

void function_init(Function *f, char *source_name) {
  f->source_name = source_name;
  f->line_defined = 0;

  f->is_vararg = 0;
  f->max_stack = 0;

  list_init(&f->params);

  list_init(&f->kstr);
  list_init(&f->knum);
  list_init(&f->kfunc);

  /*
   * Add library functions to the function's constant table.
   */
  list_append(&f->kstr, "print");
  list_append(&f->kstr, "read");
  list_append(&f->kstr, "*n");
  list_append(&f->kstr, "getn");
}

Function *new_function(char *source_name, Function *parent) {
  Function *f = malloc(sizeof(*f));
  function_init(f, source_name);
  f->parent = parent;
  return f;
}
