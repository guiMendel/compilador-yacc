#include "ast.h"
#include "compiler.h"

int main() {
  string source_name = "example.simp";
  Function main((char*) source_name.c_str(), 0);

  main.max_stack = 100;

  Identifier *id = new Identifier("number", &main);
  Identifier *result = new Identifier("result", &main);

  BlockStmt *block = new BlockStmt();

  main.statements.push_back(shared_ptr<AstNode>(new AssignExpr(result, new Number(1))));
  main.statements.push_back(shared_ptr<AstNode>(new ReadStmt(id, &main)));
  main.statements.push_back(shared_ptr<AstNode>(new WhileStmt(new BinaryExpr(BIN_GT, id, new Number(0)), block)));

  main.statements.push_back(shared_ptr<AstNode>(new WriteStmt(result, &main)));

  block->statements.push_back(shared_ptr<AstNode>(
      new AssignExpr(result, new BinaryExpr(BIN_MUL, result, id))));
  block->statements.push_back(shared_ptr<AstNode>(
      new AssignExpr(id, new BinaryExpr(BIN_SUB, id, new Number(1)))));

  compile(main);
}
