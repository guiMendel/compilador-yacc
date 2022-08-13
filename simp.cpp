#include "ast.h"
#include "compiler.h"

int main() {
  Function main("main", 0);
  compile(main);
}

