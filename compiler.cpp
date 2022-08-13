#include "compiler.h"
#include "opcodes.h"
#include "ast.h"

#include <stdio.h>
#include <string.h>

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)

class Compiler : public Visitor {
public:
  Compiler() {
    file = fopen("/tmp/simp.bin", "wb");
  }

  void compile() {

  }

  virtual void visit(Function *node) {
    emitString(node->source_name);

    emit(node->line_defined);
    emit(node->num_params);
    emit(node->is_vararg);
    emit(node->max_stack);

    emit(node->locals);
    emit(node->lines);

    emit(node->kstr);
    emit(node->knum);
    emit(node->functions);

    // TODO: figure this out
    int i = 0x00000004;
    emit(i);

    // push code
    for(auto &stmt : node->statements) {

    }
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

  void emit(int value) {
    emitBlock(&value, sizeof(value));
  }

  void emit(char* string) {
    size_t length = strlen(string) + 1;
    emit(length);
    emitBlock(string, length);
  }

  template<typename T>
  void emit(T value) {
    emit(value);
  }

  template<typename T>
  void emit(vector<T> vec) {
    emit(vec.size());
    for(auto &item : vec) {
      emit(item);
    }
  }

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
