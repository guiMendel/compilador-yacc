#include "symbol_table.h"

#include "stdio.h"
#include "stdlib.h"

char *var_used_to_string(bool);

SymbolTable *symbol_table = NULL;

SymbolTable *create_table() { return new_list(); }

void free_table(SymbolTable *t) { list_free(t); }

SymbolTableEntry *create_table_entry(char *name, VarType type) {
    SymbolTableEntry *entry = malloc(sizeof(SymbolTableEntry *));
    entry->used = false;
    entry->type = UNKNOWN;
    entry->name = name;
    return entry;
}

void display_symbol_table(SymbolTable *table) {
    SymbolTableEntry *entry = list_pop(table);

    // print table header
    char *col[] = {"NAME", "TYPE", "USED"};
    printf("| %*s | %*s | %*s |\n", -10, col[0], -10, col[1], -10, col[2]);

    while (entry != NULL) {
        // print table entry
        printf("| %-10s | %-10s | %-10s |\n", entry->name, entry_type(entry->type), var_used_to_string(entry->used));
        entry = list_pop(table);
    }
    // print table footer
    printf("\n");
}

void add_var(char *name, VarType type) {
    // TODO: Preciso fazer o free dessa entry em algum lugar?
    SymbolTableEntry *entry = create_table_entry(name, type);
    list_push(symbol_table, entry);
}

void check_var_exists(char *name) {}

bool check_var_type(char *name, VarType type) { return false; }

bool check_var_usage() { return false; }

char *entry_type(VarType type) {
    if (type == STRING) {
        return "STRING";
    }
    if (type == NUMBER) {
        return "NUMBER";
    }
    return "UNKNOWN";
}

char *var_used_to_string(bool is_used) {
    if (is_used) {
        return "TRUE";
    } else {
        return "FALSE";
    }
}