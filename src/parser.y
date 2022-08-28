%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(char *message);

#include "ast.h"
#include "list.h"
#include "compiler.h"
#include "parser.h"

static List functions;
#define fn() ((Function *)list_top(&functions))

/* int yydebug = 1; */
%}

%union {
  char *string;
  int number;
  AstNode *node;
  List *list;
  int token;
}

%token <string> ID
%token <number> NUM
%token <token> VAR IF ELSE WHILE DO END FUNCTION RETURN READ FOR EACH IN RANGE

%type <node> statements statement declaration expression function_declaration function_call
%type <list> parameters more_parameters arguments more_arguments

%right THEN ELSE

%right '='
%left OR
%left AND
%left EQUALS NOT_EQUALS
%left '<' '>'
%left '+' '-'
%left '*' '/'
%right '!'

%%
program : statements { fn()->code = $1; }
        ;

statements : empty { $$ = new_block_node(); }
           | statement statements { list_push(&$2->as_block.stmts, $1); $$ = $2; }
           | function_declaration statements { list_push(&$2->as_block.stmts, $1); $$ = $2; }
           ;

function_declaration : FUNCTION ID 
                      {
                        Function *function = new_function("=(none)", fn());
                        declareVar($2, fn());

                        /** know thyselves **/
                        declareVar($2, function);

                        list_append(&fn()->kfunc, function);
                        list_push(&functions, function);
                      }
                     '(' parameters ')' statements END 
                      { 
                        fn()->code = $7;
                        $$ = new_function_node($2, $5, (Function*)list_pop(&functions));
                      }
                     ;

parameters : empty { $$ = &fn()->params; }
           | ID more_parameters { list_push($2, $1); $$ = $2; }
           ; 

more_parameters : empty { $$ = &fn()->params; }
                | ',' ID more_parameters { list_push($3, $2); $$ = $3; }
                ;

statement : expression ';'
          | declaration ';'
          | ID '=' expression { $$ = new_assign_node($1, $3, fn()); }
          | ID '[' expression ']' '=' expression { $$ = new_array_assign_node($1, $3, $6, fn()); }
          | IF '(' expression ')' statement                   %prec THEN { $$ = new_if_node($3, $5, NULL); }
          | IF '(' expression ')' statement ELSE statement   { $$ = new_if_node($3, $5, $7); }
          | WHILE '(' expression ')' statement { $$ = new_while_node($3, $5); }
          | EACH ID IN 
            { 
              list_push(&fn()->locals, $2); // binding variable
              list_push(&fn()->locals, ""); // the index variable (not used)
              list_push(&fn()->locals, ""); // the table variable (not used)
            } 
            expression DO statements END 
            { 
              $$ = new_each_node($2, $5, $7, fn());
            }
          | FOR ID IN 
            { 
              list_push(&fn()->locals, ""); // the step variable (not used)
              list_push(&fn()->locals, ""); // the limit variable (not used)
              list_push(&fn()->locals, $2); // binding variable
            } expression RANGE expression DO statements END { $$ = new_for_node($2, $5, $7, $9); }
          | DO statements END { $$ = $2; }
          | RETURN ';' { $$ = new_return_node(NULL); }
          | RETURN expression ';' { $$ = new_return_node($2); }
          | READ ID ';' { $$ = new_read_node($2, fn()); }
          | ';' { $$ = NULL; }
          ;

declaration : VAR ID { declareVar($2, fn()); $$ = NULL; }
            | VAR ID '=' expression { declareVar($2, fn()); $$ = new_assign_node($2, $4, fn()); }
            ;

expression : '(' expression ')' { $$ = $2; }
           | function_call { $$ = $1; }
           | expression OR expression { $$ = new_binop_node(BINOP_OR, $1, $3); }
           | expression AND expression { $$ = new_binop_node(BINOP_AND, $1, $3); }
           | expression EQUALS expression { $$ = new_binop_node(BINOP_EQ, $1, $3); }
           | expression NOT_EQUALS expression { $$ = new_binop_node(BINOP_NEQ, $1, $3); }
           | expression '<' expression { $$ = new_binop_node(BINOP_LT, $1, $3); }
           | expression '>' expression { $$ = new_binop_node(BINOP_GT, $1, $3); }
           | expression '+' expression { $$ = new_binop_node(BINOP_ADD, $1, $3); }
           | expression '-' expression { $$ = new_binop_node(BINOP_SUB, $1, $3); }
           | expression '*' expression { $$ = new_binop_node(BINOP_MUL, $1, $3); }
           | expression '/' expression { $$ = new_binop_node(BINOP_DIV, $1, $3); }
           | '-' expression { $$ = new_unop_node(UNOP_NEG, $2); }
           | '!' expression { $$ = new_unop_node(UNOP_NOT, $2); }
           | '[' arguments ']' { $$ = new_array_node($2); }
           | expression '[' expression ']' { $$ = new_array_access_node($1, $3); }
           // TODO: allow call()[index] syntax
           | NUM { $$ = new_number_node($1); }
           | ID { $$ = new_ident_node($1, fn()); }
           ;

function_call : ID '(' arguments ')' { $$ = new_call_node($1, $3, fn()); }
              ;

arguments : empty { $$ = new_list(); }
          | expression more_arguments { list_push($2, $1); $$ = $2; }
          ;

more_arguments : empty { $$ = new_list(); }
               | ',' expression more_arguments { list_push($3, $2); $$ = $3; }
               ;

empty : /* empty */;
%%


void yyerror(char *message) {
  fprintf(stderr, "Error: %s at %d\n", message, yylineno);
  exit(1);
}

Function* parse(FILE *file) {
  list_init(&functions);
  list_push(&functions, new_function("=(main)", NULL));

  yyin = file;

  yyparse();

  return fn();
}
