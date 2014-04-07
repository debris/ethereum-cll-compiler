#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"

// reference: https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-CLL

int yyparse();
char *cll_read_file(char *filename);
void cll_setup_json(char *json_text);

int main(){
    char *json_text = cll_read_file("test.json");
    cll_setup_json(json_text);
    struct CLLNode *node = NULL;
    for (;;){
        yyparse(&node);
        if (node != NULL){
            int a = eval(node);
            //treefree(node);
            printf("cll >> %d\n", a);
        } else {
            printf("cll >> error root is null\n");
        }
    }
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

void cll_setup_json(char *json_text){
    json_t *root;
    json_error_t error;
    
    root = json_loads(json_text, 0, &error);

    if (!root){
        printf("error loading file!");
        return;
    }

    json_t *values, *arrays, *code;
    int i;

    values = json_object_get(root, "values");
    arrays = json_object_get(root, "arrays");
    

    for (i = 0; i < json_array_size(values); i++){
        json_t *data = json_array_get(values, i);

        json_t *name = json_object_get(data, "name");
        json_t *value = json_object_get(data, "value");
        const char *name_text = json_string_value(name);
        int value_int = json_integer_value(value);

        struct CLLSymbol *sym = cll_lookup_intval(name_text);
        sym->data.value = value_int;
    }


    json_decref(root);  // free pointer
}

void cll_setup(){
    struct CLLSymbol *cll_tx_data           = cll_lookup_array("tx.data", 2000);
    struct CLLSymbol *cll_contract_storage  = cll_lookup_array("contract.storage", 2000);
}









