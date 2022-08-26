#include "compiler.h"
#include "list.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

uint8_t *currentChunk;
int position;

#define emitByte(byte) currentChunk[position++] = byte;

/** functions stack */
list functions;
#define increaseMaxStack() ((Function *)list_top(&functions))->max_stack++;

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

static void emitList(list *l, void (*f)(void *)) {
  emit(l->size);
  list_node *n = l->head;
  while (n != NULL) {
    f(n->data);
    n = n->next;
  }
}

static void emitBlock(AstNode *node) {
  list_node *n = node->as_block.stmts.head;
  while (n != NULL) {
    emitNode(n->data);
    n = n->next;
  }
}

static int emitDummy() {
  int offset = position;
  emit(0x0);
  return offset;
}

static void patchOffset(int offset, int value) {
  int _position = position;
  position = offset;
  emit(value);
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

  int stackOffset = emitDummy(); // max_stack

  emit(0); // locals debug
  emit(0); // lines debug

  emitList(&f->kstr, (void (*)(void *))emitString);
  emitList(&f->knum, (void (*)(void *))emit);
  emitList(&f->kfunc, (void (*)(void *))emitFunction);

  int codeOffset = emitDummy(); // code

  emitNode(&f->code);

  emitLong(OP_END);

  // patch up the size
  patchOffset(codeOffset, (position - codeOffset) / 8);

  // patch up the stack size
  patchOffset(stackOffset, f->max_stack);

  list_pop(&functions);
};

static void emitBinOp(AstNode *node) {
  emitNode(node->as_binop.left);

  // increase stack
  increaseMaxStack();
  switch (node->as_binop.op) {
    case BINOP_ADD:
      if (node->as_binop.right->type == AST_NUMBER) {
        emitLong(CREATE_S(OP_ADDI, node->as_binop.right->as_number.value));
      } else {
        emit(node->as_binop.right->as_number.value);
        emitLong(OP_ADD);
      }
      break;
    default:
      break;
  }
}

static void emitNode(AstNode *node) {
  switch (node->type) {
  case AST_BLOCK:
    emitBlock(node);
    break;
  case AST_NUMBER:
    emitLong(CREATE_S(OP_PUSHINT, node->as_number.value));
    increaseMaxStack();
    break;
  case AST_RETURN:
    emitNode(node->as_ret.expr);
    emitLong(OP_RETURN);
    break;
  case AST_BINOP:
    emitBinOp(node);
    break;
  default:
    printf("Unknown node type: %d", node->type);
    break;
  }
}

void compile(Function *main, uint8_t *chunk) {
  currentChunk = chunk;
  position = 0;

  list_init(&functions);

  emitHeader();
  emitFunction(main);
}
