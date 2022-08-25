#include "ast.h"
#include "compiler.h"

void compile_iterative() {
  string source_name = "=(none)";
  Function main((char *)source_name.c_str());

  main.max_stack = 4;
  main.code = new BlockStmt();

  Identifier *id = new Identifier("number", &main);
  Identifier *result = new Identifier("result", &main);

  BlockStmt *block = new BlockStmt();
  
  main.code->statements.push_back(new VarDecl(result));
  main.code->statements.push_back(new VarDecl(id));

  main.code->statements.push_back((new AssignExpr(result, new Number(1))));
  main.code->statements.push_back((new ReadStmt(id, &main)));
  main.code->statements.push_back(
      (new WhileStmt(new BinaryExpr(BIN_GT, id, new Number(0)), block)));

  main.code->statements.push_back((new WriteStmt(result, &main)));

  block->statements.push_back(
      (new AssignExpr(result, new BinaryExpr(BIN_MUL, id, result))));
  block->statements.push_back(
      (new AssignExpr(id, new BinaryExpr(BIN_SUB, id, new Number(1)))));

  compile(&main);
}

void compile_recursive() {
  string source_name = "=(none)";
  Function *main = new Function((char *)source_name.c_str());

  main->max_stack = 100;

  Function *fac = new Function((char *)source_name.c_str());
  fac->num_params = 1;
  // TODO: params
  fac->locals.push_back("n");
  fac->max_stack = 100;

  // TODO: figure this out
  fac->kstr.push_front("fac");
  main->kstr.push_front("fac"); 

  Identifier *n = new Identifier("n", fac);
  Call *call = new Call("fac");
  call->arguments.push_back(new BinaryExpr(BIN_SUB, n, new Number(1)));

  fac->code = new BlockStmt();
  fac->code->statements.push_back(new IfStmt(
                                    new BinaryExpr(BIN_EQ, n, new Number(1)), new ReturnStmt(new Number(1)),
                                    new ReturnStmt(new BinaryExpr(BIN_MUL, call, n))));

  main->code = new BlockStmt();

  main->code->statements.push_back(new FunctionDecl("fac"));
  main->functions.push_back(fac);

  Call *call2 = new Call("fac");
  call2->arguments.push_back(new Number(5));
  main->code->statements.push_back(new WriteStmt(call2, main));

  compile(main);
}

int main() { compile_iterative(); }
