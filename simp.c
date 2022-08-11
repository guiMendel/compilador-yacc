// example code
//
// program = {
//  statements: [
//    {
//      type: "function",
//      parameters: [],
//      name: "factorial",
//      statements: [
//        ...
//      ]
//    },
//    {
//      type: "declaration", id: "a", # push nil
//    },
//    {
//      type: "read", id: "a",
//    },
//    {
//      type: "write", expression: {
//        type: "function.call",
//        id: "factorial",
//        arguments: [
//          {
//            type: "operation7", id: "a"
//          }
//        ]
//    },
// }

#include <stdio.h>
#include <stdlib.h>

#include "simp.h"

static FILE *file;

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)
#define emit(value) emitBlock(&value, sizeof(value))

static void emitHeader() {
  emitByte(ID_CHUNK);
  fputs(SIGNATURE, file);

  emitByte(0x40); // Version
  emitByte(0x01); // endianess
  emitByte(sizeof(int)); // int size
  emitByte(sizeof(size_t)); // size_t size
  emitByte(0x08); // instruction size
  emitByte(SIZE_INSTRUCTION); // instruction width
  emitByte(SIZE_OP); // op width
  emitByte(SIZE_B); // b width
  emitByte(0x08); // number size (double)

  double test = TEST_NUMBER;
  emit(test);
}

int main() {
  file = fopen("simp.bin", "wb");
  emitHeader();
  fclose(file);
  return 0;
}
