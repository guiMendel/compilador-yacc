#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "list.h"

typedef List SymbolTable;
typedef enum
{
    NUMBER,
    STRING,
} VarType;

SymbolTable* create_table();

void free_table(SymbolTable* );

void add_var(char *name, VarType type);
void check_var_exists(char *name);
bool check_var_type(char *name, VarType type);
bool check_var_usage();

#endif // SYMBOL_TABLE
