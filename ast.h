#ifndef __AST_H
#define __AST_H

#include <memory>
#include <string>
#include <vector>
#include <set>

using namespace std;

// temporary
typedef struct {
  char *name;
  int start_pc;
  int end_pc;
} Local;
//

class Visitor;

class AstNode {
public:
  virtual void accept(Visitor &visitor) = 0;
};

/*
 * A function definition
 */
class Function : public AstNode {
public:
  Function(char *name, int line) : source_name(name), line_defined(line) {}
  void accept(Visitor &visitor);

  char *source_name;
  int line_defined = 1;
  int num_params = 0;
  char is_vararg = 0;
  int max_stack = 0;

  vector<shared_ptr<AstNode>> statements;
  vector<void *> locals;
  vector<void *> lines;

  /* constants */
  vector<int> knum;
  set<string> kstr;
  vector<Function> functions;
};

class Number : public AstNode {
public:
  Number(int value) : value(value) {}
  void accept(Visitor &visitor);

  int value;
};

class Call : public AstNode {
public:
  // TODO: nosso analisador semantico vai passar pelos arguments um por um
  // então imagino que deve ser melhor a cada arg chamar
  // call.arguments.push_back do que passar os arguments no construtor
  Call(string name) : name(name) {}

  void accept(Visitor &visitor);

  string name;
  vector<shared_ptr<AstNode>> arguments;
};

class IfStmt : public AstNode {
public:
  IfStmt(AstNode *condition, AstNode *then_stmt, AstNode *else_stmt)
      : condition(condition), then_stmt(then_stmt), else_stmt(else_stmt) {}

  void accept(Visitor &visitor);

  /* condition precisa ser uma expressao para deixar algo na pilha
   * mas ainda nao possuimos esse refinamento */
  AstNode *condition;
  AstNode *then_stmt;
  AstNode *else_stmt;
};

class WhileStmt : public AstNode {
public:
  WhileStmt(AstNode *condition, AstNode *body)
      : condition(condition), body(body) {}

  void accept(Visitor &visitor);

  AstNode *condition;
  AstNode *body;
};

class BlockStmt : public AstNode {
public:
  BlockStmt() {}
  void accept(Visitor &visitor);
  vector<shared_ptr<AstNode>> statements;
};

class Identifier : public AstNode {
public:
  Identifier(string name, Function* function) : name(name) {
    function->kstr.insert(name);
  }
  void accept(Visitor &visitor);
  string name;
};

typedef enum {
  BIN_ADD,
  BIN_SUB,
  BIN_MUL,
  BIN_DIV,
  BIN_LT,
  BIN_GT,
  BIN_EQ,
  BIN_NE,
} BinaryOp;

class BinaryExpr : public AstNode {
public:
  BinaryExpr(BinaryOp op, AstNode *left, AstNode *right)
      : op(op), left(left), right(right) {}
  void accept(Visitor &visitor);
  BinaryOp op;
  AstNode *left;
  AstNode *right;
};

class AndExpr : public AstNode {
public:
  AndExpr(AstNode *left, AstNode *right) : left(left), right(right) {}
  void accept(Visitor &visitor);
  AstNode *left;
  AstNode *right;
};

class OrExpr : public AstNode {
public:
  OrExpr(AstNode *left, AstNode *right) : left(left), right(right) {}
  void accept(Visitor &visitor);
  AstNode *left;
  AstNode *right;
};

class AssignExpr : public AstNode {
public:
  AssignExpr(Identifier *left, AstNode *right) : left(left), right(right) {}
  void accept(Visitor &visitor);
  Identifier *left;
  AstNode *right;
};

class ReadStmt : public AstNode {
public:
  ReadStmt(Identifier *id, Function *function) : id(id) {
    function->kstr.insert("read");
    function->kstr.insert("*n");
  }
  void accept(Visitor &visitor);
  Identifier *id;
};

class WriteStmt : public AstNode {
public:
  WriteStmt(AstNode *expr, Function *function) : expr(expr) {
    function->kstr.insert("print");
  }
  void accept(Visitor &visitor);
  AstNode *expr;
};

class Visitor {
public:
  virtual void visit(Number *node) = 0;
  virtual void visit(Call *node) = 0;
  virtual void visit(Function *node) = 0;

  virtual void visit(IfStmt *node) = 0;
  virtual void visit(WhileStmt *node) = 0;
  virtual void visit(BlockStmt *node) = 0;
  virtual void visit(Identifier *node) = 0;

  virtual void visit(BinaryExpr *node) = 0;
  virtual void visit(AndExpr *node) = 0;
  virtual void visit(OrExpr *node) = 0;

  virtual void visit(AssignExpr *node) = 0;
  virtual void visit(ReadStmt *node) = 0;
  virtual void visit(WriteStmt *node) = 0;
};

#endif // __AST_H
