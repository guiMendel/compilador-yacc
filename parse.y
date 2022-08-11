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
%token FUNCTION
%token RETURN
%token WRITE
%token READ

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
           | function.declaration statements
           ;

function.declaration : FUNCTION ID '(' parameters ')' DO statements END
                     ;

parameters : empty | ID more.parameters;

more.parameters : empty
               | ',' ID more.parameters
               ;

statement : expression ';'
          | declaration ';'
          | IF '(' expression ')' statement                   %prec THEN
          | IF '(' expression ')' statement ELSE statement
          | WHILE '(' expression ')' statement
          | DO statements END
          | RETURN ';'
          | RETURN expression ';'
          | READ ID ';'
          | WRITE expression ';'
          | ';'
          ;

declaration : VAR ID
            ;

expression : '(' expression ')'
           | ID '=' expression
           | operation
           | function.call
           ;

function.call : ID '(' arguments ')'
              ;

arguments : empty | expression more.arguments;

more.arguments : empty
               | ',' expression more.arguments
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

empty : /* empty */;
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
