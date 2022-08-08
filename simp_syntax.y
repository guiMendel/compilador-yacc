%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(char *message);
%}

%token VAR
%token ID
%token NUM
%token IF
%token ELSE
%token WHILE
%token DO
%token END

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
program : statements
        ;

statements : empty
           | statement statements
           ;

statement : expression ';'
          | declaration ';'
          | IF '(' expression ')' statement                %prec THEN
          | IF '(' expression ')' statement ELSE statement
          | WHILE '(' expression ')' statement
          | DO statements END
          | ';'
          ;

declaration : VAR ID
            ;

expression : '(' expression ')'
           | ID '=' expression
           | operation
           ;

operation : operation OR operation
          | operation2
          ;

operation2 : operation2 AND operation2
           | operation3
           ;

operation3 : operation3 EQUALS operation3
           | operation3 NOT_EQUALS operation3
           | operation4
           ;

operation4 : operation4 '<' operation4
           | operation4 '>' operation4
           | operation5
           ;

operation5 : operation5 '+' operation5
           | operation5 '-' operation5
           | operation6
           ;

operation6 : operation6 '*' operation6
           | operation6 '/' operation6
           | operation7
           ;

operation7 : '-' operation7
           | '!' operation7
           | NUM
           | ID
           ;

/* operation : operation '+' operation
          | operation '-' operation
          | operation2
          ;

operation2 : NUM
           | ID
           | operation2 '*' operation2
           | operation2 '/' operation2
           | operation3
           ; */
           
/* operation3 :  */


empty : /* empty */
%%


void yyerror(char *message) {
      fprintf(stderr, "Error: %s\n", message);
      exit(1);
}

int main() {
      int yydebug = 1;
      
      yyparse();

      return 0;
}