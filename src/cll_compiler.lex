%{
#include "../include/cll_compiler.h"
#include "../include/tokens.h"
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
":"                     { return yytext[0]; }

">"                     { yylval.func = 1; return CMP; }
"<"                     { yylval.func = 2; return CMP; }
"<>"                    { yylval.func = 3; return CMP; }
"=="                    { yylval.func = 4; return CMP; }
">="                    { yylval.func = 5; return CMP; }       
"<="                    { yylval.func = 6; return CMP; }


"if"                    { return IF; }
"else"                  { return ELSE; }
"end"                   { return END; }
"while"                 { return WHILE; }
"array"                 { return ARRAY; }
"stop"                  { return STOP; }
<<EOF>>                 { return END_OF_FILE; }

 /* names */
[0-9]+                  { yylval.intval = atoi(yytext); return NUMBER; }

[ \t]*                    {  /* ignore whitespace */ }
[\n]                    { return EOL; }
[a-z]*\.?[a-zA-Z0-9]*       { yylval.sval = strdup(yytext); return NAME; }

%%

