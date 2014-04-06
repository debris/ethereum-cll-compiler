%error-verbose
%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/cll_compiler.h"
#include "../include/cll_ast.h"
void yyerror(char *s);
int yylex(void);
%}

%union {
    struct CLLNode *node;
    struct CLLSymbol *symbol;
    struct CLLSymlist *sl;
    int intval;
    char *sval;
}

 /* declare tokens */
%start input
%token <intval> NUMBER
%token <sval> NAME
%token EOL
%token <void> '+' '-' '*' '/' '='
%type <node> exp stmt
%left UMINUS

%%

input:
    | input stmt 
    ;


stmt: exp EOL {
        printf("cll >> %d\n", eval($1));
    }
    ;

exp: NUMBER                     { $$ = cll_newintval($1); }
   | exp '+' exp                { $$ = cll_newast('+', $1, $3); }
   | exp '-' exp                { $$ = cll_newast('-', $1, $3); }
   | exp '*' exp                { $$ = cll_newast('*', $1, $3); }
   | exp '/' exp                { $$ = cll_newast('/', $1, $3); }
   | '(' exp ')'                { $$ = $2; }
   | '-' exp %prec UMINUS       { $$ = cll_newast('M', $2, NULL); }
   | NAME                       { $$ = cll_newref(cll_lookup($1)); }
   | NAME '=' exp               { $$ = cll_newasgn(cll_lookup($1), $3); }
   ;

%%

void yyerror(char *s){
    fprintf(stderr, ": error: %s", s);
    fprintf(stderr, "\n");
}












