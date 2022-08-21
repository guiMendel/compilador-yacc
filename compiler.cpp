#include "compiler.h"
#include "ast.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)

class Compiler : public Visitor {
public:
  Compiler() { file = fopen("/tmp/simp.bin", "wb"); }

  void compile(Function &main) {
    emitHeader();
    main.accept(*this);
  }

  void visit(Function *node) {
    emit(node->source_name);

    emit(node->line_defined);
    emit(node->num_params);
    emit(node->is_vararg);
    emit(node->max_stack);

    emit(node->locals);
    emit(node->lines);

    emit(node->kstr);
    emit(node->knum);
    /* emit(node->functions); */ emit((int)0);

    instructions = 0;
    emit(0x0);
    int offset = (int)(ftell(file) - sizeof(int));

    // push code
    for (auto &stmt : node->statements) {
      stmt->accept(*this);
    }

    patch(instructions, offset);

    // FIXME: this may only be emitted on main
    emitOpCode(OP_END);
  }

  void visit(Number *node) { emitSigned(OP_PUSHINT, node->value); }

  void visit(Call *node) {
    // fetch function name
    emitUnsigned(OP_GETGLOBAL, node->index);

    // push arguments
    for (auto &arg : node->arguments) {
      arg->accept(*this);
    }

    emitOpCode(OP_CALL); // call function
  }

  void visit(IfStmt *node) {
    node->condition->accept(*this);

    int thenOffset = emitNoop();
    node->then_stmt->accept(*this);

    // talvez nao precise disso sempre
    int elseOffset = emitNoop();
    patchJump(OP_JMPF, thenOffset);

    if (node->else_stmt != nullptr) {
      node->else_stmt->accept(*this);
    }

    patchJump(OP_JMP, elseOffset);
  }

  void visit(Identifier *node) {
    emitUnsigned(OP_GETGLOBAL, node->index);
  }

private:
  FILE *file;
  int instructions;

  void emit(int value) { emitBlock(&value, sizeof(value)); }

  void emit(char value) { emitByte(value); }

  void emit(char *string) {
    size_t length = strlen(string) + 1;
    emit(length);
    emitBlock(string, length);
  }

  void emit(const char *string) {
    size_t length = strlen(string) + 1;
    emit(length);
    emitBlock(string, length);
  }

  void emit(double number) { emitBlock(&number, sizeof(number)); }

  void emit(Instruction value) { 
    emitBlock(&value, sizeof(value)); 
    instructions++;
  }

  void emit(string s) { emit(s.c_str()); }

  template <typename T> void emit(T value) { emit(value); }

  template <typename T> void emit(vector<T> vec) {
    emit((int)vec.size());
    for (auto &item : vec) {
      emit(item);
    }
  }

  void emitHeader() {
    emitByte(ID_CHUNK);
    fputs(SIGNATURE, file);

    emitByte(0x40);             // Version
    emitByte(0x01);             // endianess
    emitByte(sizeof(int));      // int size
    emitByte(sizeof(size_t));   // size_t size
    emitByte(0x08);             // instruction size
    emitByte(SIZE_INSTRUCTION); // instruction width
    emitByte(SIZE_OP);          // op width
    emitByte(SIZE_B);           // b width
    emitByte(0x08);             // number size (double)

    double test = TEST_NUMBER;
    emit(test);
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

  int emitNoop() {
    Instruction instruction = CREATE_0(OP_JMP);
    emit(instruction);
    return (int)(ftell(file) - sizeof(instruction));
  }

  void patch(int value, int offset) {
    fseek(file, offset, SEEK_SET);
    emit(value);
    fseek(file, 0, SEEK_END);
  }

  void patchJump(OpCode opcode, int offset) {
    int current = (int)(ftell(file) - sizeof(Instruction));
    int diff = (current - offset) / 8;

    fseek(file, offset, SEEK_SET);

    Instruction instruction = CREATE_S(opcode, diff);
    emit(instruction);
    instructions--; // TODO: improve this

    fseek(file, 0, SEEK_END);
  }
};

void compile(Function &main) {
  Compiler compiler;
  compiler.compile(main);
}
