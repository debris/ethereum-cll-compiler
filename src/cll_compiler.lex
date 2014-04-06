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
"="                     { return yytext[0]; }

 /* names */
[0-9]+                  { yylval.intval = atoi(yytext); return NUMBER; }

[ \t]*                  {  /* ignore whitespace */ }
[\n]                    { return EOL; }
[a-z][a-zA-Z0-9]*       { yylval.sval = strdup(yytext); return NAME; }

%%

