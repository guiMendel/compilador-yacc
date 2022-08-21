#include "ast.h"
#include "compiler.h"

int main() {
  Function main("@example.lua", 0);

  main.max_stack = 100;

  main.kstr.insert("print");
  main.kstr.insert("n");

  BlockStmt *block = new BlockStmt();

  Call *call = new Call("print");
  call->arguments.push_back(shared_ptr<AstNode>(new Identifier("n")));

  block->statements.push_back(shared_ptr<AstNode>( call ));
  block->statements.push_back(shared_ptr<AstNode>( new AssignExpr(new Identifier("n"), 
                              new BinaryExpr(BIN_SUB, new Identifier("n"), new Number(1)))));

  main.statements.push_back(shared_ptr<AstNode>(new AssignExpr(new Identifier("n"), new Number(3))));
  main.statements.push_back(shared_ptr<AstNode>(
                              new WhileStmt(new BinaryExpr(BIN_GT, new Identifier("n"), new Number(0)),
                                            block)));

  compile(main);
}

