#include <stdio.h>
#include <stdlib.h>
//#include <jansson.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"
#include "../include/cll_print.h"
#include "../include/cll_json.h"

// reference: https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-CLL

int yyparse();
char *cll_read_file(char *filename);

int main(){
    char *json_text = cll_read_file("test.json");
    cll_json_setup(json_text);
    free(json_text);
    struct CLLNode *node = NULL;
    for (;;){
        yyparse(&node);
        if (node != NULL){
            struct CLLSymbol symbol= eval(node);
            cll_print_symbol(&symbol);
            if (symbol.symboltype == CLLSymbolStop){
                break;
            }
        }

    }
    cll_json_final();
    cll_print_final();
    return EXIT_SUCCESS;
}

char *cll_read_file(char *filename){
    char *file_contents;
    long input_file_size;
    FILE *input_file = fopen(filename, "rb");
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);
    file_contents = malloc((input_file_size + 1) * (sizeof(char)));
    fread(file_contents, sizeof(char), input_file_size, input_file);
    fclose(input_file);
    file_contents[input_file_size] = 0;
    return file_contents;
}








