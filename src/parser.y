%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(char *message);

#include "ast.h"
#include "list.h"
#include "compiler.h"
#include "printer.h"
#include "scanner.h"

Function *fn;

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
%token <token> VAR IF ELSE WHILE DO END FUNCTION RETURN READ

%type <node> statements statement declaration expression operation operation2 operation3 operation4 operation5 operation6 operation7
%type <node> function.declaration function.call
%type <list> parameters more.parameters arguments more.arguments

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
program : statements { fn->code = $1; }
        ;

statements : empty { $$ = new_block_node(); }
           | statement statements { list_push(&$2->as_block.stmts, $1); $$ = $2; }
           | function.declaration statements
           ;

function.declaration : FUNCTION ID '(' parameters ')' DO statements END { $$ = NULL; }
                     ;

parameters : empty { $$ = new_list(); }
           | ID more.parameters { list_push($2, $1); $$ = $2; }
           ; 

more.parameters : empty { $$ = new_list(); }
                | ',' ID more.parameters { list_push($3, $2); $$ = $3; }
                ;

statement : expression ';'
          | declaration ';'
          | IF '(' expression ')' statement                   %prec THEN { $$ = new_if_node($3, $5, NULL); }
          | IF '(' expression ')' statement ELSE statement   { $$ = new_if_node($3, $5, $7); }
          | WHILE '(' expression ')' statement { $$ = new_while_node($3, $5); }
          | DO statements END { $$ = $2; }
          | RETURN ';' { $$ = new_return_node(NULL); }
          | RETURN expression ';' { $$ = new_return_node($2); }
          | READ ID ';' { $$ = new_read_node($2, fn); }
          | ';' { $$ = NULL; }
          ;

declaration : VAR ID { declareVar($2, fn); $$ = NULL; }
            ;

expression : '(' expression ')' { $$ = $2; }
           | ID '=' expression { $$ = new_assign_node($1, $3, fn); }
           | operation { $$ = $1; }
           | function.call { $$ = $1; }
           ;

function.call : ID '(' arguments ')' { $$ = new_call_node($1, $3, fn); }
              ;

arguments : empty { $$ = new_list(); }
          | expression more.arguments { list_push($2, $1); $$ = $2; }
          ;

more.arguments : empty { $$ = new_list(); }
               | ',' expression more.arguments { list_push($3, $2); $$ = $3; }
               ;

operation : operation OR operation { $$ = new_binop_node(BINOP_OR, $1, $3); }
          | operation2 { $$ = $1; }
          ;

operation2 : operation2 AND operation2 { $$ = new_binop_node(BINOP_AND, $1, $3); }
           | operation3 { $$ = $1; }
           ;

operation3 : operation3 EQUALS operation3 { $$ = new_binop_node(BINOP_EQ, $1, $3); }
           | operation3 NOT_EQUALS operation3 { $$ = new_binop_node(BINOP_NEQ, $1, $3); }
           | operation4 { $$ = $1; }
           ;

operation4 : operation4 '<' operation4 { $$ = new_binop_node(BINOP_LT, $1, $3); }
           | operation4 '>' operation4 { $$ = new_binop_node(BINOP_GT, $1, $3); }
           | operation5 { $$ = $1; }
           ;

operation5 : operation5 '+' operation5 { $$ = new_binop_node(BINOP_ADD, $1, $3); }
           | operation5 '-' operation5 { $$ = new_binop_node(BINOP_SUB, $1, $3); }
           | operation6 { $$ = $1; }
           ;

operation6 : expression '*' expression { $$ = new_binop_node(BINOP_MUL, $1, $3); }
           | expression '/' expression { $$ = new_binop_node(BINOP_DIV, $1, $3); }
           | operation7 { $$ = $1; }
           ;

operation7 : '-' expression { $$ = new_unop_node(UNOP_NEG, $2); }
           | '!' expression { $$ = new_unop_node(UNOP_NOT, $2); }
           | NUM { $$ = new_number_node($1); }
           | ID { $$ = new_ident_node($1, fn); }
           ;

empty : /* empty */;
%%


void yyerror(char *message) {
  fprintf(stderr, "Error: %s\n", message);
  exit(1);
}

int main() {
  Function f;
  function_init(&f, "main");
  fn = &f;

  yyparse();

  unsigned char chunk[1024];
  int size = compile(&f, chunk);

  // write the chunk to a file
  FILE *fp = fopen("simp.out", "wb");
  fwrite(chunk, 1, size, fp);
  fclose(fp);

  return 0;
}
