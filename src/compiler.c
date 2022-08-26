#include "compiler.h"
#include "list.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

uint8_t *currentChunk;
int position;

#define emitByte(byte) currentChunk[position++] = byte;

static void emitNode(AstNode *node);

static void emit(unsigned long value) {
  currentChunk[position++] = (value >> 0) & 0xff;
  currentChunk[position++] = (value >> 8) & 0xff;
  currentChunk[position++] = (value >> 16) & 0xff;
  currentChunk[position++] = (value >> 24) & 0xff;
}

static void emitSize(unsigned long long value) {
  emit(value & 0xffffffff);
  emit(value >> 32);
}

static void emitString(const char *string) {
  size_t length = strlen(string) + 1; // +1 for null terminator
  emitSize(length);
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


static void emitBlock(AstNode* node) {
  list_node *n = node->block.stmts.head;
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
  /* functions.push(fn); */

  emitString(f->source_name);

  emit(f->line_defined);
  emit(f->num_params);
  emitByte(f->is_vararg);
  emit(f->max_stack);

  emit(0); // locals debug
  emit(0); // lines debug

  emitList(&f->kstr, (void (*)(void *))emitString);
  emitList(&f->knum, (void (*)(void *))emit);
  emitList(&f->kfunc, (void (*)(void *))emitFunction);

  int offset = position;
  emit(0xffffffff);

  emitNode(&f->block);

  emitSize(OP_END);

  // patch up the size
  int diff = (position - offset) / 8;
  position = offset;
  emit(diff);
  position += diff * 8;

  /* functions.pop(); */
};

static void emitNode(AstNode* node) {
  switch (node->type) {
    case AST_BLOCK:
      emitBlock(node);
      break;
  }
}

void compile(Function *main, uint8_t *chunk) {
  currentChunk = chunk;
  position = 0;

  emitHeader();
  emitFunction(main);
}
