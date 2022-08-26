#include "compiler.h"
#include "list.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

uint8_t *currentChunk;
int position;

/** functions stack */
list functions;

#define emitByte(byte) currentChunk[position++] = byte;

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

  int stackOffset = position;
  emit(f->max_stack);

  emit(0); // locals debug
  emit(0); // lines debug

  emitList(&f->kstr, (void (*)(void *))emitString);
  emitList(&f->knum, (void (*)(void *))emit);
  emitList(&f->kfunc, (void (*)(void *))emitFunction);

  int offset = position;
  emit(0xffffffff);

  emitNode(&f->code);

  emitLong(OP_END);

  // patch up the size
  int diff = (position - offset) / 8;
  position = offset;
  emit(diff);
  position += diff * 8;

  // patch up the stack size
  // save position
  int pos = position;
  position = stackOffset;
  emit(f->max_stack);
  position = pos;

  list_pop(&functions);
};

static void emitNode(AstNode *node) {
  switch (node->type) {
  case AST_BLOCK:
    emitBlock(node);
    break;
  case AST_NUMBER:
    emitLong(CREATE_S(OP_PUSHINT, node->as_number.value));
    ((Function *)list_pop(&functions))->max_stack++;
    break;
  case AST_RETURN:
    emitNode(node->as_ret.expr);
    emitLong(OP_RETURN);
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
