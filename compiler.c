#include "compiler.h"
#include "opcodes.h"

#include <string.h>

uint8_t *currentChunk;
int position;

#define emitByte(byte) currentChunk[position++] = byte;

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
  emit(f->is_vararg);
  emit(f->max_stack);

  emit(0); // locals debug
  emit(0); // lines debug

  emitList(&f->kstr, (void (*)(void *))emitString);
  emitList(&f->knum, (void (*)(void *))emit);
  emitList(&f->kfunc, (void (*)(void *))emitFunction);

  emit(0x0);
  int offset = position;

  /* emitBlock(fn->block); */

  emit(OP_END);

  // patch up the size
  currentChunk[offset - 1] = position - offset;

  /* functions.pop(); */
};

void compile(Function *main, uint8_t *chunk) {
  currentChunk = chunk;
  position = 0;

  emitHeader();
  emitFunction(main);
}
