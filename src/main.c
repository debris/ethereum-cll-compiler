#include <stdio.h>
#include <stdlib.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"

int yyparse();

int main(){
    yyparse();
    return EXIT_SUCCESS;
}
