#ifndef __AST_H
#define __AST_H

#include <vector>
#include <string>

using namespace std;

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

  char* source_name;
  int line_defined;
  int num_params;
  char is_vararg;
  int max_stack;

  vector<AstNode> statements;
  vector<void*> locals;
  vector<void*> lines;

  /* constants */
  vector<int> knum;
  vector<string> kstr;
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
  Call(int index, vector<AstNode> arguments)
      : index(index), arguments(arguments) {}

  void accept(Visitor &visitor);

  /** index of this function on kstr vector */
  int index;
  vector<AstNode> arguments;
};


class Visitor {
public:
  virtual void visit(Number *node) = 0;
  virtual void visit(Call *node) = 0;
};

#endif // __AST_H
