#include "ast.h"

void Number::accept(Visitor &v) { v.visit(this); }
void Call::accept(Visitor &v) { v.visit(this); }
void Function::accept(Visitor &v) { v.visit(this); }

