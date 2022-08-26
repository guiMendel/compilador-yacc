#include "greatest.h"

SUITE_EXTERN(suite_block);
SUITE_EXTERN(suite_return);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_block);
  RUN_SUITE(suite_return);
  GREATEST_MAIN_END();
}
