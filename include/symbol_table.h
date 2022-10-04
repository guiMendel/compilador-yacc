#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

extern int yylineno;

typedef List SymbolTable;
typedef struct Function Function;
typedef enum { NUMBER, STRING, UNKNOWN, PROCEDURE } VarType;

typedef struct SymbolTableEntry {
  bool used;
  VarType type;
  char *name;
  int index;
} SymbolTableEntry;

typedef List ErrorList;
typedef List WarningList;

typedef struct ErrorEntry {
  char *message;
} ErrorEntry;

typedef ErrorEntry WarningEntry;

void init_aux_tables();

void var_assignment(SymbolTable *, char *, VarType, int *);
void var_read(Function *, char *, int *, int *);
void var_add(SymbolTable *, char *, VarType);

void procedure_read(SymbolTable *, char *);
void procedure_add(SymbolTable *, char *);

// errors and warning
void variable_not_declared(char *);
void variable_already_declared(char *);
void variable_not_used(char *name);
void assign_value_type_different(char *);
void procedure_not_used(char *name);
void procedure_not_declared(char *);
void procedure_already_declared(char *);
void check_variable_not_used(SymbolTable *symbol_table);
void check_procedure_not_used(SymbolTable *symbol_table);
bool has_semantic_errors();
bool has_semantic_warnings();
void display_warning_list();
void display_error_list();

SymbolTableEntry *find_table_entry(SymbolTable *symbol_table, char *name);
void display_symbol_table(SymbolTable *);

bool is_system_global_procedure(char *);

char *entry_type(VarType);

void free_table(SymbolTable *);

#endif // SYMBOL_TABLE
