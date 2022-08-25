#ifndef COMPILER_H
#define COMPILER_H

#include <stdint.h>

#include "ast.h"

void compile(Function *main, uint8_t* chunk);

#endif

