#include "symbol_table.h"
#include "stdlib.h"

SymbolTable* create_table(){
    return new_list();
}

void free_table(SymbolTable* t){
    list_free(t);
}

void add_var(char *name, int type){

}

void check_var_exists(char *name){

}

bool check_var_type(char *name, int type){
    return false;
}

bool check_var_usage(){
    return false;
}