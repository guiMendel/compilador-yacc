#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef enum { DECLARATION, USE } VarUsage;
bool should_display_verbose;

void display_identifier_use(int, char*, VarUsage);

#endif  // VERBOSE
