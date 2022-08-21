#include "ast.h"
#include "compiler.h"

int main() {
  Function main("@example.lua", 0);

  main.max_stack = 100;

  main.kstr.push_back("print");
  main.kstr.push_back("n");

  BlockStmt *block = new BlockStmt();

  Call *call = new Call(0);
  BinaryExpr *cond = new BinaryExpr(BIN_GT, new Identifier(1), new Number(0));
  WhileStmt *while_stmt = new WhileStmt(cond, block);

  call->arguments.push_back(shared_ptr<AstNode>(new Number(3)));
  block->statements.push_back(shared_ptr<AstNode>(call));

  main.statements.push_back(shared_ptr<AstNode>(while_stmt));

  compile(main);
}

