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
%token <void> '=' ':' '[' ']' IF END ELSE ARRAY WHILE EOL SEND ','
%token <intval> END_OF_FILE STOP
%type <node> input exp stmt stmts cond
%nonassoc <func> CMP
%left '+' '-'
%left '*' '/' '%'
%left '^' 
%left UMINUS

%%

input:
     stmt {
        (*(struct CLLNode**)astDest) = $1;
        YYACCEPT;
     } 
     ;

stmts:                                  { $$ = cll_newstmts(); }
     | stmts stmt                       { $$ = cll_addstmt($1, $2); }
     ;
     

stmt: exp EOL                           { $$ = $1;}
    | EOL stmt                          { $$ = $2;}
    | END_OF_FILE                       { $$ = cll_newstop($1);}
    | WHILE exp ':' EOL stmts END EOL   { $$ = cll_newwhile($2, $5); }
    | IF exp ':' EOL stmts END EOL      { $$ = cll_newif($2, $5, NULL); }
    | IF exp ':' EOL stmts ELSE ':' EOL stmts END EOL { $$ = cll_newif($2, $5, $9); }
    | SEND '(' exp ',' exp ',' exp ')' EOL              { $$ = cll_newsend(cll_lookup_transactions(), $3, $5, $7); }
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
   | NAME                               { $$ = cll_newref(cll_lookup_intval($1)); }
   | NAME '=' exp                       { $$ = cll_newasgn(cll_lookup_intval($1), $3); }
   | exp CMP exp                        { $$ = cll_newcmp($2, $1, $3); }
   | NAME '=' ARRAY '(' NUMBER ')'      { $$ = cll_newref(cll_lookup_array($1, $5));}
   | NAME '[' exp ']'                   { $$ = cll_newarray_access(cll_lookup_array($1, 0), $3);}
   | NAME '[' exp ']' '=' exp           { $$ = cll_newarray_asgn(cll_lookup_array($1, 0), $3, $6);}
   | STOP                               { $$ = cll_newstop($1);}
   ;

cond: NUMBER                    { $$ = cll_newintval($1); }
    ;


%%

void yyerror(char *s){
    fprintf(stderr, ": error: %s", s);
    fprintf(stderr, "\n");
}












