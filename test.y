/* Lexical tokens */
%token ID
%token INTCON
%token CHARCON
%token STRINGCON

/* Terminal tokens */
%token EXTERN
%token VOID
%token CHAR
%token INT
%token IF
%token ELSE
%token WHILE
%token FOR
%token RETURN

/* Symbols starting with more. mean 0 or more repetitions of whatever they refer to, usually preceded by a comma */
/* Symbols starting with optional mean empty or what they refer to */

%%
/* ======== FROM C-- SYNTAX PAGE ======== */
/* https://www2.cs.arizona.edu/~debray/Teaching/CSc453/DOCS/cminusminusspec.html */

program : empty
        | program.line
        | program program.line
        ;

        
program.line : declaration ';'
             | function
             ;

declaration : type variable.declaration more.variable.declaration
            | optional.extern type ID '(' parameters ')' more.declaration
            | optional.extern VOID ID '(' parameters ')' more.declaration
            ;

variable.declaration : ID optional.array.size

type : CHAR | INT;

parameters : VOID
           | type ID optional.array.indicator more.parameters

function : type ID '(' parameters ')' '{' variable.declaration.lines statements '}'
         | VOID ID '(' parameters ')' '{' variable.declaration.lines statements '}'
         ;

statement : IF '(' expression ')' statement optional.else
          | WHILE '(' expression ')' statement
          | FOR '(' optional.assignment ';' optional.expression ';' optional.assignment ')' statement
          | RETURN optional.expression ';'
          | assignment ';'
          | ID '(' optional.expressions ')' ';'
          | '{' statement '}'
          | ';'
          ;

assignment : ID optional.bracket.expression '=' expression;

expression : '-' expression
           | '!' expression
           | expression binary.operation expression
           | expression relative.operation expression
           | expression logical.operation expression
           | ID function.or.array.parameters
           | '(' expression ')'
           | INTCON
           | CHARCON
           | STRINGCON
           ;

binary.operation : '+' | '-' | '*' | '/';

relative.operation : '=''=' | '!''=' | '<''=' | '>''=' | '<' | '>';

logical.operation : '&''&' | '|''|';


/* ======== EXTRA SYMBOLS ======== */

variable.declaration.lines : empty
                           | type variable.declaration more.variable.declaration ';'
                           | variable.declaration.lines variable.declaration.lines
                           ;

statements : empty
           | statement
           | statements statements
           ;

function.or.array.parameters : empty
                             | '(' optional.expressions ')'
                             | '[' expression ']'
                             ;

/* ======== EPSILON SYMBOL ======== */

empty : /* nothing to see here */
      ;

/* ======== OPTIONAL SYMBOLS ======== */

optional.array.size : empty | '[' INTCON ']'

optional.array.indicator : empty |  '[' ']'

optional.extern : empty | EXTERN;

optional.else : empty | ELSE statement;

optional.expression : empty | expression;

optional.assignment : empty | assignment;

optional.expressions : empty | expression more.expression;

optional.bracket.expression : empty | '[' expression ']'

/* ======== REPETITION SYMBOLS ======== */

more.declaration : empty
                | ',' ID '(' parameters ')'
                | more.declaration more.declaration
                ;

more.variable.declaration : empty
                              | ',' variable.declaration
                              | more.variable.declaration more.variable.declaration
                              ;

more.parameters : empty
                | ',' type ID optional.array.indicator
                | more.parameters more.parameters
                ;

more.expression : empty
                | ',' expression
                | more.expression more.expression
                ;
%%