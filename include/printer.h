#ifndef PRINTER_H
#define PRINTER_H

#include "ast.h"

void printFunction(Function *function);
void printNode(AstNode *node, int depth);

#endif // PRINTER_H
