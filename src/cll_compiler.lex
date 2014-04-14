%{
#include "../include/cll_compiler.h"
#include "../include/tokens.h"

int cll_line_number = 1;
%}

%%


 /* single character ops */
"+" |
"-" |
"*" |
"/" |
"(" |
")" |
"%" |
"^" |
"=" |
"[" |
"]" |
"," |
":"                     { return yytext[0]; }

">"                     { yylval.func = 1; return CMP; }
"<"                     { yylval.func = 2; return CMP; }
"<>"                    { yylval.func = 3; return CMP; }
"=="                    { yylval.func = 4; return CMP; }
">="                    { yylval.func = 5; return CMP; }       
"<="                    { yylval.func = 6; return CMP; }
"||"                    { yylval.func = 7; return CMP; }
"or"                    { yylval.func = 7; return CMP; }
"&&"                    { yylval.func = 8; return CMP; }
"and"                   { yylval.func = 8; return CMP; }


"if"                    { return IF; }
"else"                  { return ELSE; }
"end"                   { return END; }
"while"                 { return WHILE; }
"array"                 { return ARRAY; }
"stop"                  { yylval.intval = cll_line_number; return STOP; }
<<EOF>>                 { yylval.intval = cll_line_number; return END_OF_FILE; }
"send"                  { return SEND; }

 /* names */
[0-9]+                  { yylval.intval = atoi(yytext); return NUMBER; }

[ \t]*                    {  /* ignore whitespace */ }
[\n]                     { cll_line_number++; return EOL; }
[a-zA-Z]*\.?[_a-zA-Z0-9]*       { yylval.sval = strdup(yytext); return NAME; }

%%

