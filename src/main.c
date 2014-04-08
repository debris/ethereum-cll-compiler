#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"
#include "../include/cll_print.h"
#include "../include/cll_json.h"

// reference: https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-CLL

int yyparse();
char *cll_read_file(const char *filename);

void cll_show_help(){
    printf("Ethereum contract compiler:\n");
    printf("Options:\n");
    printf("[-i]        | input json\n");
    printf("[-o]        | output json\n");
    printf("[-h]        | show help\n");
    printf("if none is provided, command line mode is enabled.\n");
}

int main(int argc, char *argv[]){
    char *input_file = NULL;
    char *output_file = NULL;

    int i;
    for (i = 1; i < argc; ++i){
        if (!strcmp(argv[i], "-i")){
            input_file = strdup(argv[++i]); 
        } else if (!strcmp(argv[i], "-o")){
            output_file = strdup(argv[++i]);
        } else if (!strcmp(argv[i], "-h")){
            cll_show_help();
            return EXIT_SUCCESS;
        } else {
            printf("error, unknown param: %s\n", argv[i]);
        }
    }

    if (argc == 1){
        printf("Command line mode: \n");
    }

    if (input_file){
        char *json_text = cll_read_file(input_file);
        cll_json_setup(json_text);
        free(json_text);
    }

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

    if (output_file){
        cll_json_final(output_file);
    } else {
        cll_print_final();
    }
    return EXIT_SUCCESS;
}

char *cll_read_file(const char *filename){
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








