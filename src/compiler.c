#include "compiler.h"
#include "list.h"
#include "opcodes.h"
#include "symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *currentChunk;
int position;
unsigned long long lastInstruction;

#define emitByte(byte) currentChunk[position++] = byte;

/** functions stack */
static List functions;

#define currentFunction() ((Function *)list_top(&functions))

static void handlePush() {
  currentFunction()->depth++;
  if (currentFunction()->depth > currentFunction()->max_stack) {
    currentFunction()->max_stack = currentFunction()->depth;
  }
}

static void handlePop() { currentFunction()->depth--; }

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
  lastInstruction = value;
}

static void emitString(const char *string) {
  size_t length = strlen(string) + 1; // +1 for null terminator
  emitLong(length);
  for (int i = 0; i < length; i++) {
    emitByte(string[i]);
  }
}

static void emitLocal(const char *string) {
  emitString(string);
  emit(0);    // init pc
  emit(0xff); // last pc
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
  f->depth = f->params.size;

  emitString(f->source_name);

  emit(f->line_defined + functions.size - 1);
  emit(f->params.size);
  emitByte(f->is_vararg);

  int stackOffset = emitDummy(sizeof(int)); // max_stack

  emit(f->params.size + f->locals.size);
  while (f->params.size > 0) {
    emitLocal(list_pop(&f->params));
  }
  while (f->locals.size > 0) {
    emitLocal(list_pop(&f->locals));
  }
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
    } else {
      emitNode(right);
      emitLong(OP_ADD);
      handlePop();
    }
    break;
  case BINOP_SUB:
    if (node->as_binop.right->type == AST_NUMBER) {
      emitLong(CREATE_S(OP_ADDI, -right->as_number.value));
    } else {
      emitNode(right);
      emitLong(OP_SUB);
      handlePop();
    }
    break;
  case BINOP_MUL:
    emitNode(right);
    emitLong(OP_MULT);
    handlePop();
    break;
  case BINOP_DIV:
    emitNode(right);
    emitLong(OP_DIV);
    handlePop();
    break;
  case BINOP_EQ:
    emitNode(right);
    emitCompare(OP_JMPEQ);
    handlePop();
    break;
  case BINOP_NEQ:
    emitNode(right);
    emitCompare(OP_JMPNE);
    handlePop();
    break;
  case BINOP_LT:
    emitNode(right);
    emitCompare(OP_JMPLT);
    handlePop();
    break;
  case BINOP_GT:
    emitNode(right);
    emitCompare(OP_JMPGT);
    handlePop();
    break;
  case BINOP_AND:;
    int elseOffset = emitDummy(sizeof(Instruction));
    handlePop();
    emitNode(right);
    patchJump(elseOffset, OP_JMPONF);
    break;
  case BINOP_OR:;
    int thenOffset = emitDummy(sizeof(Instruction));
    handlePop();
    emitNode(right);
    patchJump(thenOffset, OP_JMPONT);
    break;
  }
}

static void emitNode(AstNode *node) {
  if (node == NULL) {
    emitLong(CREATE_U(OP_PUSHNIL, 1));
    handlePush();
    return;
  }

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
    handlePop();
    emitLong(CREATE_U(OP_RETURN, currentFunction()->depth));
    break;
  case AST_BINOP:
    emitBinOp(node);
    break;
  case AST_UNOP:
    emitNode(node->as_unop.expr);
    switch (node->as_unop.op) {
    case UNOP_NEG:
      emitLong(OP_MINUS);
      break;
    case UNOP_NOT:
      emitLong(OP_NOT);
      break;
    }
    handlePop();
    break;
  case AST_ASSIGN: {
    emitNode(node->as_assign.expr);
    if (!node->as_assign.is_decl) {
      emitLong(CREATE_U(OP_SETLOCAL, node->as_assign.index));
      handlePop();
    }
    break;
  }
  case AST_IDENT: {
    int index, is_upvalue;
    var_read(currentFunction(), node->as_ident.name, &index, &is_upvalue);
    emitLong(CREATE_U(is_upvalue ? OP_PUSHUPVALUE : OP_GETLOCAL, index));
    handlePush();
    break;
  }
  case AST_IF: {
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
  }
  case AST_WHILE: {
    int startOffset = position;

    emitNode(node->as_while.cond);

    int endOffset = emitDummy(sizeof(Instruction));
    handlePop();

    emitNode(node->as_while.body);

    emitLong(CREATE_S(OP_JMP, (startOffset - position) / 8 - 1));
    patchJump(endOffset, OP_JMPF);
    break;
  }
  case AST_CALL: {
    emitNode(node->as_call.expr);
    handlePop();

    int _depth = currentFunction()->depth;

    handlePush();
    emitNodeList(node->as_call.args);

    int has_return = 1;

    emitLong(CREATE_AB(OP_CALL, _depth, has_return));

    currentFunction()->depth = _depth + has_return;
    break;
  }
  case AST_READ:
    // TODO: this should not be hardcoded
    emitLong(CREATE_U(OP_GETGLOBAL, 1));
    emitLong(CREATE_U(OP_PUSHSTRING, 2));

    emitLong(CREATE_AB(OP_CALL, currentFunction()->depth, 1));

    emitLong(CREATE_U(OP_SETLOCAL, node->as_read.index));
    break;
  case AST_PRINT:
    emitLong(CREATE_U(OP_GETGLOBAL, 0));

    int _depth = currentFunction()->depth;

    handlePush();
    emitNode(node->as_print.expr);

    emitLong(CREATE_AB(OP_CALL, _depth, 0));

    currentFunction()->depth = _depth;
    break;
  case AST_FUNCTION: {
    // emit upvalues
    list_node *u = node->as_function.fn->upvalues.head;
    while (u != NULL) {
      emitLong(CREATE_U(OP_GETLOCAL, *(int *)u->data));
      free(u->data);
      u = u->next;
    }

    emitLong(CREATE_AB(OP_CLOSURE, node->as_function.fn_index,
                       node->as_function.fn->upvalues.size));
    handlePush();
    break;
  }
  }
}

int compile(Function *main, char *chunk) {
  currentChunk = chunk;
  position = 0;

  list_init(&functions);

  emitHeader();
  emitFunction(main);

  return position;
}
