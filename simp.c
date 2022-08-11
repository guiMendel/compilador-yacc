#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simp.h"
#include "opcodes.h"

// placeholder for parse
static void parse(Function *main) { 
  main->source_name = "=(none)";
  main->line_defined = 1;
  main->num_params = 0;
  main->is_vararg = 0;
  main->max_stack = 200;
  list_init(&main->statements);
  list_init(&main->functions);
  list_init(&main->locals);
  list_init(&main->lines);
  list_init(&main->knum);
  list_init(&main->kstr);
  list_init(&main->code);

  char *str = "print";
  list_push(&main->kstr, str);

  AstNode *node = malloc(sizeof(AstNode));
  node->type = NODE_CALL;
  node->val = 0; // index on constants
  list_init(&node->children);

  AstNode *child = malloc(sizeof(AstNode));
  child->type = NODE_NUM;
  child->val = 3;

  list_push(&node->children, child);
  list_push(&main->statements, node);
}

static FILE *file;

#define emitVector(vector, count) fwrite(vector, sizeof(*vector), count, file)
#define emitBlock(pointer, size) fwrite(pointer, size, 1, file)
#define emitByte(byte) fputc(byte, file)
#define emit(value) emitBlock(&value, sizeof(value))

static void emitSigned(OpCode opcode, int value) {
  Instruction instruction = CREATE_S(opcode, value);
  emit(instruction);
}

static void emitUnsigned(OpCode opcode, unsigned value) {
  Instruction instruction = CREATE_U(opcode, value);
  emit(instruction);
}

static void emitOpCode(OpCode opcode) {
  Instruction instruction = CREATE_0(opcode);
  emit(instruction);
}

static void emitString(const char *string) {
  size_t length = strlen(string) + 1;
  emit(length);
  emitBlock(string, length);
}

static void emitFunction(Function *function);
static void handleNodes(list* nodes);

static void emitHeader() {
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

static void handleNodes(list* nodes) {
  // iterate through statements
  for(list_node* node = nodes->head; node != NULL; node = node->next) {
    AstNode* ast_node = node->data;
    switch(ast_node->type) {
      case NODE_NUM: {
        emitSigned(OP_PUSHINT, ast_node->val);
        break;
      }
      case NODE_CALL:
        emitUnsigned(OP_GETGLOBAL, ast_node->val); // fetch function name
        handleNodes(&ast_node->children); // push arguments
        emitOpCode(OP_CALL); // call function
        break;
      default:
        printf("Not implemented: %d\n", ast_node->type);
        break;
    }
  }
}

static void emitLocals(list* locals) {
  emit(locals->size);
  for(list_node* node = locals->head; node != NULL; node = node->next) {
    Local* local = node->data;
    emitString(local->name);
    emit(local->start_pc);
    emit(local->end_pc);
  }
}

static void emitLines(list* lines) {
  emit(lines->size);
  // TODO: emit line?
}

static void emitStrings(list* strings) {
  emit(strings->size);
  for(list_node* node = strings->head; node != NULL; node = node->next) {
    char* string = node->data;
    emitString(string);
  }
}

static void emitNumbers(list* numbers) {
  emit(numbers->size);
  for(list_node* node = numbers->head; node != NULL; node = node->next) {
    int *number = node->data;
    emit(*number);
  }
}

static void emitFunctions(list* functions) {
  emit(functions->size);
  for(list_node* node = functions->head; node != NULL; node = node->next) {
    Function* function = node->data;
    emitFunction(function);
  }
}

static void emitFunction(Function* function) {
  emitString(function->source_name);

  emit(function->line_defined);
  emit(function->num_params);
  emit(function->is_vararg);
  emit(function->max_stack);

  emitLocals(&function->locals);
  emitLines(&function->lines);
  emitStrings(&function->kstr);
  emitNumbers(&function->knum);
  emitFunctions(&function->functions);

  // TODO: figure this out
  int i = 0x00000004;
  emit(i);

  handleNodes(&function->statements);
}

int main() {
  file = fopen("simp.bin", "wb");
  // parse will come from parse.y
  // program is a top level function
  Function main;
  parse(&main);

  emitHeader();
  emitFunction(&main);

  Instruction end = CREATE_0(OP_END);
  emit(end);

  fclose(file);
  return 0;
}
