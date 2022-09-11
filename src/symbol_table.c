#include "symbol_table.h"

#include "stdio.h"
#include "stdlib.h"

char *var_used_to_string(bool);
SymbolTableEntry *find_variable(char *);

SymbolTable *symbol_table = NULL;
ErrorList *error_list = NULL;
WarningList *warning_list = NULL;

SymbolTable *create_table() {
    SymbolTable *symbol_table = new_list();
    error_list = new_list();
    warning_list = new_list();

    return symbol_table;
}

void free_table(SymbolTable *t) {
    list_free(error_list);
    list_free(warning_list);
    list_free(t);
}

ErrorEntry *create_error_entry(char *content) {
    ErrorEntry *entry = malloc(sizeof(ErrorEntry *));
    entry->message = content;
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
bool has_semantic_warnings() { return warning_list->size > 0; }

void display_error_list() {
    ErrorEntry *entry = list_pop(error_list);

    printf("-- ERRORS --\n");

    while (entry != NULL) {
        // print table entry
        printf("%s\n\n", entry->message);
        entry = list_pop(error_list);
    }
}

void display_warning_list() {
    WarningEntry *entry = list_pop(warning_list);

    printf("-- WARNINGS --\n");

    while (entry != NULL) {
        // print table entry
        printf("%s\n\n", entry->message);
        entry = list_pop(warning_list);
    }
}

char *interpolate_error(char *template, char *content) {
    char *str = malloc(sizeof(char *) * 200);
    sprintf(str, template, content, yylineno);
    return str;
}

void add_var(char *name, VarType type) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry == NULL) {
        SymbolTableEntry *symbol_entry = create_table_entry(name, type);
        list_push(symbol_table, symbol_entry);
    } else {
        variable_already_declared(name);
        return;
    }
}

void var_read(char *name) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry != NULL) {
        entry->used = true;
    } else {
        variable_not_declared(name);
        return;
    }
}

void var_assignment(char *name, VarType type) {
    SymbolTableEntry *entry = find_variable(name);
    if (entry != NULL) {
        if (entry->type == UNKNOWN || entry->type == type) {
            entry->type = type;
        } else {
            assign_value_type_different(name);
            return;
        }
        entry->used = true;
    } else {
        variable_not_declared(name);
        return;
    }
}

void check_variable_not_used() {
    list_node *n = symbol_table->head;
    bool not_used = false;

    if (symbol_table->size == 0) {
        return;
    }
    while (n != NULL) {
        SymbolTableEntry *entry = (SymbolTableEntry *)n->data;
        not_used = !entry->used && entry->type != PROCEDURE;

        if (not_used) {
            variable_not_used(entry->name);
            not_used = false;
        }

        n = n->next;
    }
    return;
}

void check_procedure_not_used() {
    list_node *n = symbol_table->head;
    bool not_used = false;

    if (symbol_table->size == 0) {
        return;
    }
    while (n != NULL) {
        SymbolTableEntry *entry = (SymbolTableEntry *)n->data;
        not_used = !entry->used && entry->type == PROCEDURE;

        if (not_used) {
            procedure_not_used(entry->name);
            not_used = false;
        }

        n = n->next;
    }
    return;
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

char *entry_type(VarType type) {
    if (type == STRING) {
        return "STRING";
    }
    if (type == NUMBER) {
        return "NUMBER";
    }
    if (type == PROCEDURE) {
        return "PROCEDURE";
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

void variable_not_used(char *name) {
    char *template = "Variable %s, at line %d is not being used.";
    char *str = interpolate_error(template, name);
    WarningEntry *entry = create_error_entry(str);
    list_push(warning_list, entry);
}

void procedure_not_used(char *name) {
    char *template = "Procedure %s, at line %d is not being used.";
    char *str = interpolate_error(template, name);
    WarningEntry *entry = create_error_entry(str);
    list_push(warning_list, entry);
}

void variable_not_declared(char *name) {
    char *template = "Variable %s, at line %d is not declared.";
    char *str = interpolate_error(template, name);
    ErrorEntry *entry = create_error_entry(str);
    list_push(error_list, entry);
}

void procedure_not_declared(char *name) {
    char *template = "Procedure %s, at line %d is not declared.";
    char *str = interpolate_error(template, name);
    ErrorEntry *entry = create_error_entry(str);
    list_push(error_list, entry);
}

void variable_already_declared(char *name) {
    char *template = "Variable %s, at line %d, is already declared.";
    char *str = interpolate_error(template, name);
    ErrorEntry *entry = create_error_entry(str);
    list_push(error_list, entry);
}

void procedure_already_declared(char *name) {
    char *template = "Procedure %s, at line %d, is already declared.";
    char *str = interpolate_error(template, name);
    ErrorEntry *entry = create_error_entry(str);
    list_push(error_list, entry);
}

void assign_value_type_different(char *name) {
    char *template =
        "Assiging a value of type different to variable %s, at line "
        "%d.";
    char *str = interpolate_error(template, name);
    ErrorEntry *entry = create_error_entry(str);
    list_push(error_list, entry);
}