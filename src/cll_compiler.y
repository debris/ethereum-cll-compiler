%error-verbose
%{
#define YYPARSE_PARAM astDest

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
    int func;
    char *sval;
}

 /* declare tokens */
%start input
%token <intval> NUMBER
%token <sval> NAME 
%token <void> '+' '-' '*' '/' '%' '^' '=' ':' '[' ']' IF STOP EOL BREAK ELSE ARRAY
%type <node> input exp stmt stmts cond
%left UMINUS
%nonassoc <func> CMP

%%

input:
     stmt {
        (*(struct CLLNode**)astDest) = $1;
        YYACCEPT;
     } 
     ;

stmts:                          { $$ = cll_newstmts(); }
     | stmts stmt               { $$ = cll_addstmt($1, $2); }
     ;
     

stmt: exp EOL                   { $$ = $1;}
    | IF exp ':' EOL stmts BREAK EOL { $$ = cll_newflow('I', $2, $5, NULL); }
    | IF exp ':' EOL stmts ELSE ':' EOL stmts BREAK EOL { $$ = cll_newflow('I', $2, $5, $9); }
    ;

exp: NUMBER                             { $$ = cll_newintval($1); }
   | exp '+' exp                        { $$ = cll_newast('+', $1, $3); }
   | exp '-' exp                        { $$ = cll_newast('-', $1, $3); }
   | exp '*' exp                        { $$ = cll_newast('*', $1, $3); }
   | exp '/' exp                        { $$ = cll_newast('/', $1, $3); }
   | exp '%' exp                        { $$ = cll_newast('%', $1, $3); }
   | exp '^' exp                        { $$ = cll_newast('^', $1, $3); }
   | '(' exp ')'                        { $$ = $2; }
   | '-' exp %prec UMINUS               { $$ = cll_newast('M', $2, NULL); }
   | NAME                               { $$ = cll_newref(cll_lookup(0, $1, 0)); }
   | NAME '=' exp                       { $$ = cll_newasgn(cll_lookup(0, $1, 0), $3); }
   | exp CMP exp                        { $$ = cll_newcmp($2, $1, $3); }
   | NAME '=' ARRAY '(' NUMBER ')'      { $$ = cll_newref(cll_lookup(1, $1, $5));}
   | NAME '[' exp ']'                   { $$ = cll_newarray_access(cll_lookup(1, $1, 0), $3);}
   | NAME '[' exp ']' '=' exp           { $$ = cll_newarray_asgn(cll_lookup(1, $1, 0), $3, $6);}
   | STOP                               { $$ = cll_newstop();}
   ;

cond: NUMBER                    { $$ = cll_newintval($1); }
    ;


%%

void yyerror(char *s){
    fprintf(stderr, ": error: %s", s);
    fprintf(stderr, "\n");
}












