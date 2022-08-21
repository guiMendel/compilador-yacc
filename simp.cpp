#include "ast.h"
#include "compiler.h"

int main() {
  Function main("@example.lua", 0);

  main.max_stack = 100;

  Identifier *id = new Identifier("number", &main);

  BlockStmt *block = new BlockStmt();


  block->statements.push_back(shared_ptr<AstNode>(new WriteStmt(id, &main)));
  block->statements.push_back(shared_ptr<AstNode>( new AssignExpr(id, 
                              new BinaryExpr(BIN_SUB, id, new Number(1)))));

  main.statements.push_back(shared_ptr<AstNode>(new ReadStmt(id, &main)));
  main.statements.push_back(shared_ptr<AstNode>(
                              new WhileStmt(new BinaryExpr(BIN_GT, id, new Number(0)),
                                            block)));

  compile(main);
}

