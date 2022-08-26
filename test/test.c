#include "greatest.h"

SUITE_EXTERN(suite_block);
SUITE_EXTERN(suite_return);
SUITE_EXTERN(suite_binop);
SUITE_EXTERN(suite_var);
SUITE_EXTERN(suite_if);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_block);
  RUN_SUITE(suite_return);
  RUN_SUITE(suite_binop);
  RUN_SUITE(suite_var);
  RUN_SUITE(suite_if);
  GREATEST_MAIN_END();
}
