#include "ast.h"
#include "compiler.h"

int main() {
  Function main("@example.lua", 0);

  main.max_stack = 100;

  main.kstr.push_back("print");
  Call *call = new Call(0);
  Number *number = new Number(3);
  call->arguments.push_back(shared_ptr<AstNode>(number));

  main.statements.push_back(shared_ptr<AstNode>(call));

  compile(main);
}

