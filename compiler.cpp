#include "opcodes.h"
#include <stdio.h>
#include <vector>

using namespace std;

class Visitor;

class AstNode {
public:
  virtual void accept(Visitor &visitor) = 0;
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

void Number::accept(Visitor &v) { v.visit(this); }

void Call::accept(Visitor &v) { v.visit(this); }

class Compiler : public Visitor {
public:
  Compiler() {
    file = fopen("simp.bin", "wb");
  }

  void compile() {

  }

  virtual void visit(Number *node) { emitSigned(OP_PUSHINT, node->value); }

  virtual void visit(Call *node) {
    // fetch function name
    emitUnsigned(OP_GETGLOBAL, node->index);

    // push arguments
    for (auto &arg : node->arguments) {
      arg.accept(*this);
    }

    emitOpCode(OP_CALL); // call function
  }

private:
  static FILE *file;

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)
#define emit(value) emitBlock(&value, sizeof(value))

  void emitSigned(OpCode opcode, int value) {
    Instruction instruction = CREATE_S(opcode, value);
    emit(instruction);
  }

  void emitUnsigned(OpCode opcode, int value) {
    Instruction instruction = CREATE_U(opcode, value);
    emit(instruction);
  }

  void emitOpCode(OpCode opcode) {
    Instruction instruction = CREATE_0(opcode);
    emit(instruction);
  }
};
