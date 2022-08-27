#include "greatest.h"
#include "compiler.h"

unsigned char while_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x61,
  0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x98, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xfe, 0xff, 0x7f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int while_out_len = 142;

TEST while_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode ident;
  ast_ident_init(&ident, "var");

  AstNode number;
  ast_number_init(&number, 1);

  AstNode add;
  ast_binop_init(&add, BINOP_SUB, &ident, &number);

  AstNode assign;
  ast_assign_init(&assign, "var", &add);

  AstNode while_node;
  ast_while_init(&while_node, &ident, &assign);

  list_push(&f.kstr, ident.as_ident.name);
  list_push(&f.code.as_block.stmts, &while_node);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(while_out, chunk, while_out_len);
  PASS();
}

extern SUITE(suite_while) {
  RUN_TEST(while_program);
}
