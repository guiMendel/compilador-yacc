#include "symbol_table.h"

#include "stdio.h"
#include "stdlib.h"

char *var_used_to_string(bool);
SymbolTableEntry *find_variable(char *);

SymbolTable *symbol_table = NULL;
ErrorList *error_list = NULL;

SymbolTable *create_table() {
    SymbolTable *symbol_table = new_list();
    error_list = new_list();

    return symbol_table;
}

void free_table(SymbolTable *t) {
    list_free(error_list);
    list_free(t);
}

ErrorEntry *create_error_entry(char *content) {
    ErrorEntry *entry = malloc(sizeof(ErrorEntry *));
    entry->error = content;
    return entry;
}

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
        printf("| %-10s | %-10s | %-10s |\n", entry->name,
               entry_type(entry->type), var_used_to_string(entry->used));
        entry = list_pop(table);
    }
    // print table footer
    printf("\n");
}

bool has_semantic_errors() { return error_list->size > 0; }

void display_error_list() {
    ErrorEntry *entry = list_pop(error_list);

    while (entry != NULL) {
        // print table entry
        printf("%s\n", entry->error);
        entry = list_pop(error_list);
    }
}

char *interpolate_error(char *template, char *content) {
    char *str = malloc(sizeof(char *) * 200);
    sprintf(str, template, content);
    return str;
}

void add_var(char *name, VarType type) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry == NULL) {
        SymbolTableEntry *symbol_entry = create_table_entry(name, type);
        list_push(symbol_table, symbol_entry);
    } else {
        // should print to error
        char *template = "Variable %s is already declared\n";
        char *str = interpolate_error(template, name);
        ErrorEntry *error = create_error_entry(str);
        list_push(error_list, error);
    }
}

void var_read(char *name) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry != NULL) {
        entry->used = true;
    } else {
        // should print to error
        char *template = "Variable %s is not declared\n";
        char *str = interpolate_error(template, name);
        ErrorEntry *error = create_error_entry(str);
        list_push(error_list, error);
        return;
    }
}

void var_assignment(char *name, VarType type) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry != NULL) {
        if (entry->type == UNKNOWN || entry->type == type) {
            entry->type = type;
        } else {
            char *template =
                "Assiging a value of type different from variable %s\n";
            char *str = interpolate_error(template, name);
            ErrorEntry *error = create_error_entry(str);
            list_push(error_list, error);
            return;
        }
        entry->used = true;
    } else {
        // should print to error
        char *template = "Variable %s is not declared\n";
        char *str = interpolate_error(template, name);
        ErrorEntry *error = create_error_entry(str);
        list_push(error_list, error);
        return;
    }
}

SymbolTableEntry *find_variable(char *name) {
    list_node *n = symbol_table->head;
    bool has_found = false;

    if (symbol_table->size == 0) {
        return NULL;
    }
    while (n != NULL) {
        SymbolTableEntry *entry = (SymbolTableEntry *)n->data;
        has_found = strcmp(name, entry->name) == 0;

        if (has_found) {
            return (SymbolTableEntry *)n->data;
        }

        n = n->next;
    }
    return NULL;
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