#include <stdio.h>
#include <stdlib.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"

int yyparse();

int main(){
    struct CLLNode *node = NULL;
    for (;;){
        yyparse(&node);
        if (node != NULL){
            int a = eval(node);
            treefree(node);
            printf("cll >> %d\n", a);
        } else {
            printf("cll >> error root is null\n");
        }
    }
    return EXIT_SUCCESS;
}
