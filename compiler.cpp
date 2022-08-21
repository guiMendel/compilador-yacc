#include "compiler.h"
#include "ast.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)
#define getOffset(pointer) (int)(ftell(file) - sizeof(Instruction))

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

    emit(0x0);
    int offset = (int)(ftell(file) - sizeof(int));

    // push code
    for (auto &stmt : node->statements) {
      stmt->accept(*this);
    }

    // FIXME: this may only be emitted on main
    emitOpCode(OP_END);

    // write how many bytes were written
    int diff = (int)(ftell(file) - sizeof(int) - offset) / 8;
    patch(diff, offset);
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

    int thenOffset = emitJump();
    node->then_stmt->accept(*this);

    // talvez nao precise disso sempre
    int elseOffset = emitJump();
    patchJump(OP_JMPF, thenOffset);

    if (node->else_stmt != nullptr) {
      node->else_stmt->accept(*this);
    }

    patchJump(OP_JMP, elseOffset);
  }

  void visit(WhileStmt *node) {
    int conditionOffset = getOffset();

    node->condition->accept(*this);
    int exitOffset = emitJump();
    node->body->accept(*this);

    int loopOffset = getOffset();
    int diff = (conditionOffset - loopOffset) / 8 ;
    Instruction loopJump = CREATE_S(OP_JMP, diff - 1);
    emit(loopJump);

    patchJump(OP_JMPF, exitOffset);
  }

  void visit(BlockStmt *node) {
    for (auto &stmt : node->statements) {
      stmt->accept(*this);
    }
  }

  void visit(Identifier *node) {
    emitUnsigned(OP_GETGLOBAL, node->index);
  }

  void visit(BinaryExpr *node) {
    node->left->accept(*this);
    node->right->accept(*this);

    switch(node->op) {
      case BinaryOp::BIN_ADD: emitOpCode(OP_ADD); break;
      case BinaryOp::BIN_SUB: emitOpCode(OP_SUB); break;
      case BinaryOp::BIN_MUL: emitOpCode(OP_MULT); break;
      case BinaryOp::BIN_DIV: emitOpCode(OP_DIV); break;
      case BinaryOp::BIN_LT: emitCompare(OP_JMPLT); break;
      case BinaryOp::BIN_GT: emitCompare(OP_JMPGT); break;
      case BinaryOp::BIN_EQ: emitCompare(OP_JMPEQ); break;
      case BinaryOp::BIN_NE: emitCompare(OP_JMPNE); break;
    }
  }

  void visit(AndExpr *node) {
    node->left->accept(*this);
    int elseOffset = emitJump();
    node->right->accept(*this);
    patchJump(OP_JMPONF, elseOffset);
  }

  void visit(OrExpr *node) {
    node->left->accept(*this);
    int thenOffset = emitJump();
    node->right->accept(*this);
    patchJump(OP_JMPONT, thenOffset);
  }

private:
  FILE *file;

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

  void emitCompare(OpCode opcode) {
    Instruction instruction = CREATE_S(opcode, 1);
    emit(instruction);
    instruction = CREATE_0(OP_PUSHNILJMP);
    emit(instruction);
    instruction = CREATE_S(OP_PUSHINT, 1);
    emit(instruction);
  }

  int emitJump() {
    Instruction instruction = CREATE_0(OP_JMP);
    emit(instruction);
    return getOffset();
  }

  void patch(int value, int offset) {
    fseek(file, offset, SEEK_SET);
    emit(value);
    fseek(file, 0, SEEK_END);
  }

  void patchJump(OpCode opcode, int offset) {
    int current = getOffset();
    int diff = (current - offset) / 8;

    fseek(file, offset, SEEK_SET);

    Instruction instruction = CREATE_S(opcode, diff);
    emit(instruction);

    fseek(file, 0, SEEK_END);
  }
};

void compile(Function &main) {
  Compiler compiler;
  compiler.compile(main);
}
