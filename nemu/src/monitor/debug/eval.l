%{
#include "eval-parse.h"
#include "nemu.h"
extern int read_input_for_lexer(char*, unsigned long *, int);
#undef YY_INPUT
#define YY_INPUT(b,r,s) read_input_for_lexer(b,&r,s)
%}
INT [0-9]+
HEX 0x[0-9a-f]+
REG \$e?(([a-d]x|sp|bp|si|di|ip)|([a-d](h|l)))
WHITE	[ \t\n]|(\r\n)
EQ "=="
NEQ "!="
AND "&&"
OR "||"
%%
{INT}		{ sscanf(yytext, "%d", yylval); return NUMBER; }
{HEX}		{ sscanf(yytext, "%x", yylval); return NUMBER; }
{REG}		{
    int len = strlen(yytext), i;
    char *reg = yytext + 1;
    int found = 0;
    for (i = 0; i < 8; i++) {
        if (strncmp(reg, regsl[i], len) == 0) {
            *yylval = reg_l(i);
            found = 1;
        } else if (strncmp(reg, regsw[i], len) == 0) {
            *yylval = reg_w(i);
            found = 1;
        } else if (strncmp(reg, regsb[i], len) == 0) {
            *yylval = reg_b(i);
            found = 1;
        } else if (strncmp(reg, "eip", len) == 0) {
            *yylval = cpu.eip;
            found = 1;
        }
    }
    if (!found) {
        *yylval = 0;
        printf("Invalid register name %s\n", yytext);
    }
    return NUMBER;
}
{EQ}    { return EQ; }
{NEQ}    { return NEQ; }
{AND}    { return AND; }
{OR}    { return OR; }
{WHITE}		{ /* do nothing */ }
.			{ return yytext[0];	}
%%
