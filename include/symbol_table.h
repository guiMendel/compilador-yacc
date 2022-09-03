#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef List SymbolTable;
typedef enum { NUMBER, STRING, UNKNOWN } VarType;

void var_assignment(char *, VarType);

typedef struct SymbolTableEntry {
    bool used;
    VarType type;
    char* name;
} SymbolTableEntry;

SymbolTable* symbol_table;

SymbolTable* create_table();

void free_table(SymbolTable*);

void display_symbol_table(SymbolTable*);

char* entry_type(VarType);

void add_var(char*, VarType);
void check_var_exists(char*);
bool check_var_type(char*, VarType);
bool check_var_usage();

#endif  // SYMBOL_TABLE
