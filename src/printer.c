#include "compiler.h"
#include "list.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

void printNode(AstNode *node, int depth) {
  printf("%*s", depth * 2, "");
  switch (node->type) {
  case AST_BLOCK:;
    list_node *n = node->as_block.stmts.head;
    while (n != NULL && n->data != NULL) { // TODO: the second condition should not be necessary
      printNode(n->data, depth + 1);
      n = n->next;
    }
    break;
  case AST_NUMBER:
    printf("Number: %d\n", node->as_number.value);
    break;
  case AST_RETURN:
    printf("Return: \n");
    printNode(node->as_ret.expr, depth + 1);
    break;
  case AST_BINOP:
    printf("Binop: %d \n", node->as_binop.op);
    printNode(node->as_binop.left, depth + 1);
    printNode(node->as_binop.right, depth + 1);
    break;
  case AST_ASSIGN:
    printf("Assign: %s \n", node->as_assign.name);
    printNode(node->as_assign.expr, depth + 1);
    break;
  case AST_IDENT:
    printf("Ident: %s \n", node->as_ident.name);
    break;
  case AST_IF:
    printf("If: \n");
    printNode(node->as_if.cond, depth + 1);
    printNode(node->as_if.then, depth + 1);
    if (node->as_if.els != NULL) {
      printf("%*s", depth * 2, "");
      printf("Else: \n");
      printNode(node->as_if.els, depth + 1);
    }
    break;

  case AST_WHILE:
    printf("While: \n");
    printNode(node->as_while.cond, depth + 1);
    printNode(node->as_while.body, depth + 1);
    break;

  case AST_UNOP:
    printf("Unop: %d \n", node->as_unop.op);
    printNode(node->as_unop.expr, depth + 1);
    break;
  }
}

void printFunction(Function *function) {
  printf("function %s\n\n", function->source_name);
  printNode(function->code, 0);
}
