#ifndef COMPILER_H
#define COMPILER_H

#include <stdint.h>

#include "ast.h"

int compile(Function *main, uint8_t* chunk);

#endif

