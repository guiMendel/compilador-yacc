#include "compiler.h"
#include "list.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

uint8_t *currentChunk;
int position;

#define emitByte(byte) currentChunk[position++] = byte;

/** functions stack */
List functions;

/*
 * This is used to keep track of the number of values on the stack.
 *
 * This may not be the best aproach, as branches "branch" the stack. The
 * alternative is to pass the depth as a parameter to the functions that emit
 * code.
 **/
int depth;

#define currentFunction() ((Function *)list_top(&functions))

static void handlePush() {
  depth++;
  if (depth > currentFunction()->max_stack) {
    currentFunction()->max_stack = depth;
  }
}

static void handlePop() { depth--; }

static void emitNode(AstNode *node);

static void emit(unsigned long value) {
  currentChunk[position++] = (value >> 0) & 0xff;
  currentChunk[position++] = (value >> 8) & 0xff;
  currentChunk[position++] = (value >> 16) & 0xff;
  currentChunk[position++] = (value >> 24) & 0xff;
}

static void emitLong(unsigned long long value) {
  emit(value & 0xffffffff);
  emit(value >> 32);
}

static void emitString(const char *string) {
  size_t length = strlen(string) + 1; // +1 for null terminator
  emitLong(length);
  for (int i = 0; i < length; i++) {
    emitByte(string[i]);
  }
}

static void emitList(List *l, void (*f)(void *)) {
  emit(l->size);
  list_node *n = l->head;
  while (n != NULL) {
    f(n->data);
    n = n->next;
  }
}

static void emitNodeList(List *list) {
  list_node *n = list->head;
  while (n != NULL) {
    emitNode(n->data);
    n = n->next;
  }
}

static int emitDummy(size_t size) {
  int offset = position;
  for (int i = 0; i < size; i++) {
    emitByte(0x0);
  }
  return offset;
}

static void patchOffset(int offset, int value) {
  int _position = position;
  position = offset;
  emit(value);
  position = _position;
}

static void patchJump(int offset, OpCode op) {
  int _position = position;
  int diff = position - offset - 8;
  position = offset;
  emitLong(CREATE_S(op, diff / 8));
  position = _position;
}

static void emitHeader() {
  emitByte(0x1b);
  emitByte('L');
  emitByte('u');
  emitByte('a');

  emitByte(0x40);             // Version
  emitByte(0x01);             // endianess
  emitByte(sizeof(int));      // int size
  emitByte(sizeof(size_t));   // size_t size
  emitByte(0x08);             // instruction size
  emitByte(SIZE_INSTRUCTION); // instruction width
  emitByte(SIZE_OP);          // op width
  emitByte(SIZE_B);           // b width
  emitByte(0x08);             // number size (double)

  // emit test number (double PI)
  emit(0xa15be612);
  emit(0x41b2b9b0);
}

static void emitFunction(Function *f) {
  list_push(&functions, f);

  emitString(f->source_name);

  emit(f->line_defined);
  emit(f->num_params);
  emitByte(f->is_vararg);

  int stackOffset = emitDummy(sizeof(int)); // max_stack

  emit(0); // locals debug
  emit(0); // lines debug

  emitList(&f->kstr, (void (*)(void *))emitString);
  emitList(&f->knum, (void (*)(void *))emit);
  emitList(&f->kfunc, (void (*)(void *))emitFunction);

  int codeOffset = emitDummy(sizeof(int)); // code

  emitNode(f->code);

  emitLong(OP_END);

  // patch up the size
  patchOffset(codeOffset, (position - codeOffset) / 8);

  // patch up the stack size
  patchOffset(stackOffset, f->max_stack);

  list_pop(&functions);
};

static void emitCompare(OpCode opcode) {
  Instruction instruction = CREATE_S(opcode, 1);
  emitLong(instruction);
  instruction = CREATE_0(OP_PUSHNILJMP);
  emitLong(instruction);
  instruction = CREATE_S(OP_PUSHINT, 1);
  emitLong(instruction);
}

static void emitBinOp(AstNode *node) {
  emitNode(node->as_binop.left);

  AstNode *right = node->as_binop.right;

  switch (node->as_binop.op) {
  case BINOP_ADD:
    if (right->type == AST_NUMBER) {
      emitLong(CREATE_S(OP_ADDI, right->as_number.value));
      handlePush();
    } else {
      emitNode(right);
      emitLong(OP_ADD);
    }
    break;
  case BINOP_SUB:
    if (node->as_binop.right->type == AST_NUMBER) {
      emitLong(CREATE_S(OP_ADDI, -right->as_number.value));
      handlePush();
    } else {
      emitNode(right);
      emitLong(OP_SUB);
    }
    break;
  case BINOP_MUL:
    emitNode(right);
    emitLong(OP_MULT);
    break;
  case BINOP_DIV:
    emitNode(right);
    emitLong(OP_DIV);
    break;
  case BINOP_EQ:
    emitNode(right);
    emitCompare(OP_JMPEQ);
    break;
  case BINOP_NEQ:
    emitNode(right);
    emitCompare(OP_JMPNE);
    break;
  case BINOP_LT:
    emitNode(right);
    emitCompare(OP_JMPLT);
    break;
  case BINOP_GT:
    emitNode(right);
    emitCompare(OP_JMPGT);
    break;
  default:
    break;
  }

  handlePop();
}

static void emitNode(AstNode *node) {
  if (node == NULL)
    return;

  switch (node->type) {
  case AST_BLOCK:
    emitNodeList(&node->as_block.stmts);
    break;
  case AST_NUMBER:
    emitLong(CREATE_S(OP_PUSHINT, node->as_number.value));
    handlePush();
    break;
  case AST_RETURN:
    emitNode(node->as_ret.expr);
    emitLong(OP_RETURN);
    handlePop();
    break;
  case AST_BINOP:
    emitBinOp(node);
    break;
  case AST_ASSIGN:
    emitNode(node->as_assign.expr);
    emitLong(CREATE_U(OP_SETGLOBAL, node->as_assign.index));
    handlePop();
    break;
  case AST_IDENT:
    emitLong(CREATE_U(OP_GETGLOBAL, node->as_ident.index));
    handlePush();
    break;
  case AST_IF:
    emitNode(node->as_if.cond);
    handlePop();

    int thenOffset = emitDummy(sizeof(Instruction));
    emitNode(node->as_if.then);

    if (node->as_if.els != NULL) {
      int endOffset = emitDummy(sizeof(Instruction));
      patchJump(thenOffset, OP_JMPF);

      emitNode(node->as_if.els);
      patchJump(endOffset, OP_JMP);
    } else {
      patchJump(thenOffset, OP_JMPF);
    }

    break;
  case AST_WHILE:;
    int startOffset = position;

    emitNode(node->as_while.cond);
    handlePop();

    int endOffset = emitDummy(sizeof(Instruction));
    emitNode(node->as_while.body);

    emitLong(CREATE_S(OP_JMP, (startOffset - position) / 8 - 1));
    patchJump(endOffset, OP_JMPF);
    break;
  case AST_CALL:
    // fetch function name
    emitLong(CREATE_U(OP_GETGLOBAL, node->as_call.index));
    int _depth = depth;

    depth++;
    emitNodeList(node->as_call.args);

    emitLong(CREATE_AB(OP_CALL, _depth, 1));

    depth--;
    break;
  }
}

int compile(Function *main, uint8_t *chunk) {
  currentChunk = chunk;
  position = 0;
  depth = 0;

  list_init(&functions);

  emitHeader();
  emitFunction(main);

  return position;
}
