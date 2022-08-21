#include "ast.h"
#include "compiler.h"

int main() {
  Function main("=(none)", 0);

  main.max_stack = 100;

  main.kstr.push_back("print");

  Call *call = new Call(0);
  BinaryExpr *expr = new BinaryExpr(BIN_ADD, new Number(3), new Number(1));
  BinaryExpr *comp = new BinaryExpr(BIN_EQ, expr, new Number(4));
  IfStmt *if_stmt = new IfStmt(comp, call, call);
  call->arguments.push_back(shared_ptr<AstNode>(expr));

  main.statements.push_back(shared_ptr<AstNode>(if_stmt));

  compile(main);
}

