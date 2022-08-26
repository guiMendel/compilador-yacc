#include "greatest.h"
#include "compiler.h"

static unsigned char add_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
  0x00, 0x00, 0x46, 0x0a, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x58, 0x0a,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static unsigned int add_luac_out_len = 106;

TEST add_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 42);

  AstNode num2;
  ast_number_init(&num2, 42);

  AstNode add;
  ast_binop_init(&add, BINOP_ADD, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &add);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(add_luac_out, chunk, add_luac_out_len);
  PASS();
}

unsigned char subi_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
  0x00, 0x00, 0x46, 0x0a, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x58, 0xf5,
  0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int subi_luac_out_len = 106;

TEST sub_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 42);

  AstNode num2;
  ast_number_init(&num2, 42);

  AstNode add;
  ast_binop_init(&add, BINOP_SUB, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &add);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(subi_luac_out, chunk, subi_luac_out_len);
  PASS();
}

unsigned char mul_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x46, 0x0a, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x0a,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int mul_luac_out_len = 114;

TEST mul_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 42);

  AstNode num2;
  ast_number_init(&num2, 42);

  AstNode mul;
  ast_binop_init(&mul, BINOP_MUL, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &mul);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(mul_luac_out, chunk, mul_luac_out_len);
  PASS();
}

unsigned char div_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x46, 0x0a, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x0a,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int div_luac_out_len = 114;

TEST div_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 42);

  AstNode num2;
  ast_number_init(&num2, 42);

  AstNode div;
  ast_binop_init(&div, BINOP_DIV, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &div);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(div_luac_out, chunk, div_luac_out_len);
  PASS();
}

unsigned char eq_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0xc6, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int eq_luac_out_len = 130;

TEST eq_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 4);

  AstNode num2;
  ast_number_init(&num2, 2);

  AstNode eq;
  ast_binop_init(&eq, BINOP_EQ, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &eq);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(eq_luac_out, chunk, eq_luac_out_len);
  PASS();
}

unsigned char neq_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0xc6, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int neq_luac_out_len = 130;

TEST neq_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 4);

  AstNode num2;
  ast_number_init(&num2, 2);

  AstNode neq;
  ast_binop_init(&neq, BINOP_NEQ, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &neq);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(neq_luac_out, chunk, neq_luac_out_len);
  PASS();
}

unsigned char gt_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0xc6, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int gt_luac_out_len = 130;

TEST gt_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 4);

  AstNode num2;
  ast_number_init(&num2, 2);

  AstNode gt;
  ast_binop_init(&gt, BINOP_GT, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &gt);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(gt_luac_out, chunk, gt_luac_out_len);
  PASS();
}

unsigned char lt_luac_out[] = {
  0x1b, 0x4c, 0x75, 0x61, 0x40, 0x01, 0x04, 0x08, 0x08, 0x20, 0x06, 0x09,
  0x08, 0x12, 0xe6, 0x5b, 0xa1, 0xb0, 0xb9, 0xb2, 0x41, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x29,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0xc6, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int lt_luac_out_len = 130;

TEST lt_program(void) {
  Function f;
  function_init(&f, "=(none)");

  AstNode num1;
  ast_number_init(&num1, 4);

  AstNode num2;
  ast_number_init(&num2, 2);

  AstNode lt;
  ast_binop_init(&lt, BINOP_LT, &num1, &num2);

  AstNode ret;
  ast_return_init(&ret, &lt);

  list_push(&f.code.as_block.stmts, &ret);

  unsigned char chunk[1024];
  compile(&f, chunk);

  ASSERT_MEM_EQ(lt_luac_out, chunk, lt_luac_out_len);
  PASS();
}

extern SUITE(suite_binop) {
  RUN_TEST(add_program);
  RUN_TEST(sub_program);
  RUN_TEST(mul_program);
  RUN_TEST(div_program);

  RUN_TEST(eq_program);
  RUN_TEST(neq_program);
  RUN_TEST(gt_program);
  RUN_TEST(lt_program);
};

