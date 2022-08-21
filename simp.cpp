#include "ast.h"
#include "compiler.h"

int main() {
  Function main("=(none)", 0);

  main.max_stack = 100;

  main.kstr.push_back("true");
  main.kstr.push_back("print");

  Call *call = new Call(1);
  Identifier *id = new Identifier(0);
  Number *number = new Number(2);
  IfStmt *if_stmt = new IfStmt(id, call, call);
  call->arguments.push_back(shared_ptr<AstNode>(number));

  main.statements.push_back(shared_ptr<AstNode>(if_stmt));

  compile(main);
}

