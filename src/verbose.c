#include "verbose.h"

bool should_display_verbose = false;

void display_identifier_use(int line, char* name, VarUsage usage) {
    if (!should_display_verbose) {
        return;
    }
    if (usage == DECLARATION) {
        printf("Variable %s was declared at %d", name, line);
    }
    if (usage == USE) {
        printf("Variable %s is being used at %d", name, line);
    }
}