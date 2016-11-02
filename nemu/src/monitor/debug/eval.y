%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nemu.h"
int32_t eval_result;
int eval_error;
%}

%pure-parser
%lex-param {void * scanner}
%parse-param {void * scanner}
%token NUMBER EQ
%left OR
%left AND
%left EQ NEQ
%left '-' '+'
%left '*' '/'
%right '!' DEREF UMINUS

%%
statement: expression { eval_result = $1; };
expression:    expression '+' expression    { $$ = $1 + $3; }
    |    expression '-' expression    { $$ = $1 - $3; }
    |    expression '*' expression    { $$ = $1 * $3; }
    |    expression '/' expression    { $$ = $1 / $3; }
    |    expression EQ expression    { $$ = ($1 == $3); }
    |    expression NEQ expression    { $$ = ($1 != $3); }
    |    expression AND expression    { $$ = ($1 && $3); }
    |    expression OR expression    { $$ = ($1 || $3); }
    |    '!' expression { $$ = !$2; }
    |    '*' expression %prec DEREF    { $$ = (YYSTYPE)swaddr_read((swaddr_t)$2, sizeof(uint32_t)); }
    |    '-' expression %prec UMINUS    { $$ = - $2; }
    |    '(' expression ')'        { $$ = $2; }
    |    NUMBER                { $$ = $1; }
    ;
%%
int yyerror(void* lexer, char *msg) {
    eval_error = 1;
    return fprintf(stderr, "Eval error: %s\n", msg);
}
