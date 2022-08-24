#include "compiler.h"
#include "ast.h"
#include "opcodes.h"

#include <stack>
#include <stdio.h>
#include <string.h>

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)
#define getOffset(pointer) (int)(ftell(file) - sizeof(Instruction))

class Compiler : public Visitor {
public:
  Compiler() { file = fopen("/tmp/simp.bin", "wb"); }

  void compile(Function *main) {
    emitHeader();
    emit(main);
  }

  void visit(Number *node) { emitSigned(OP_PUSHINT, node->value); }

  void visit(Call *node) {
    // fetch function name
    emitUnsigned(OP_GETGLOBAL, findGlobal(node->name));

    depth++;

    // push arguments
    for (auto &arg : node->arguments) {
      arg->accept(*this);
    }

    emit(CREATE_AB(OP_CALL, depth, 1));

    depth--;
  }

  void visit(FunctionDecl *node) {
    // TODO: handle upvalues
    emit(CREATE_AB(OP_CLOSURE, findGlobal(node->name), 0));
    emitUnsigned(OP_SETGLOBAL, findGlobal(node->name));
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
    int diff = (conditionOffset - loopOffset) / 8;
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
    emitUnsigned(OP_GETLOCAL, findLocal(node->name));
  }

  void visit(BinaryExpr *node) {
    node->left->accept(*this);
    node->right->accept(*this);

    switch (node->op) {
    case BinaryOp::BIN_ADD:
      emitOpCode(OP_ADD);
      break;
    case BinaryOp::BIN_SUB:
      emitOpCode(OP_SUB);
      break;
    case BinaryOp::BIN_MUL:
      emitOpCode(OP_MULT);
      break;
    case BinaryOp::BIN_DIV:
      emitOpCode(OP_DIV);
      break;
    case BinaryOp::BIN_LT:
      emitCompare(OP_JMPLT);
      break;
    case BinaryOp::BIN_GT:
      emitCompare(OP_JMPGT);
      break;
    case BinaryOp::BIN_EQ:
      emitCompare(OP_JMPEQ);
      break;
    case BinaryOp::BIN_NE:
      emitCompare(OP_JMPNE);
      break;
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

  void visit(AssignExpr *node) {
    node->right->accept(*this);
    emitUnsigned(OP_SETLOCAL, findLocal(node->left->name));
  }

  void visit(WriteStmt *node) {
    emitUnsigned(OP_GETGLOBAL, findGlobal("print"));

    node->expr->accept(*this);

    emitOpCode(OP_CALL);
  }

  void visit(ReadStmt *node) {
    emitUnsigned(OP_GETGLOBAL, findGlobal("read"));
    emitUnsigned(OP_PUSHSTRING, findGlobal("*n"));

    // read has multiple return, so we need to call it with 1 argument
    emit(CREATE_AB(OP_CALL, 0, 1));

    emitUnsigned(OP_SETLOCAL, findLocal(node->id->name));
  }

  void visit(ReturnStmt *node) {
    if (node->expr) node->expr->accept(*this);
    emitUnsigned(OP_RETURN, 1);
  }

private:
  FILE *file;
  stack<Function *> functions;

  /*
   * depth of function call on the stack
   */
  int depth = 0;

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

  void emit(Instruction value) { emitBlock(&value, sizeof(value)); }

  void emit(string s) { emit(s.c_str()); }
  
  void emit(Function *fn) {
    functions.push(fn);

    emit(fn->source_name);

    emit(fn->line_defined);
    emit(fn->num_params);
    emit(fn->is_vararg);
    emit(fn->max_stack);

    /* emit(fn->locals); */
    emit((int) fn->locals.size()); // workaround while locals aren't correct
    for (auto &item : fn->locals) {
      emit(item);
      emit((int) 0);
      emit((int) 0xff);
    }

    emit(fn->lines);

    emit(fn->kstr);
    emit(fn->knum);
    emit(fn->functions);

    emit(0x0);
    int offset = (int)(ftell(file) - sizeof(int));

    fn->code->accept(*this);

    emitOpCode(OP_END);

    // write how many bytes were written
    int diff = (int)(ftell(file) - sizeof(int) - offset) / 8;
    patch(diff, offset);

    functions.pop();
  }

  void emit(Local *l) {
    emit(l->name);
    emit(l->start_pc);
    emit(l->end_pc);
  }

  template <typename T> void emit(T value) { emit(value); }

  template <typename T> void emit(vector<T> vec) {
    emit((int)vec.size());
    for (auto &item : vec) {
      emit(item);
    }
  }

  template <typename T> void emit(set<T> s) {
    emit((int)s.size());
    for (auto &item : s) {
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

  int findLocal(string name) {
    auto index = functions.top()->locals.find(name);

    if (index == functions.top()->locals.end()) {
      printf("%s not found\n", name.c_str());
      exit(1);
    }

    return distance(functions.top()->locals.begin(), index);
  }

  int findGlobal(string name) {
    auto index = functions.top()->kstr.find(name);

    if (index == functions.top()->kstr.end()) {
      printf("%s not found\n", name.c_str());
      exit(1);
    }

    return distance(functions.top()->kstr.begin(), index);
  }
};

void compile(Function *main) {
  Compiler compiler;
  compiler.compile(main);
}
