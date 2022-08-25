#include "ast.h"
#include "list.h"

void function_init(Function *f, char *source_name) {
  f->source_name = source_name;
  f->line_defined = 0;
  f->num_params = 0;
  f->is_vararg = 0;
  f->max_stack = 0;

  list_init(&f->kstr);
  list_init(&f->knum);
  list_init(&f->kfunc);
}
