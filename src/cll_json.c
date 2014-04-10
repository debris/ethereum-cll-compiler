#include <stdio.h>
#include <jansson.h>
#include "../include/cll_json.h"
#include "../include/cll_ast.h"

void yy_scan_string();  // fix this!

void cll_json_setup(char *json_text){
    json_t *root;
    json_error_t error;
    
    root = json_loads(json_text, 0, &error);

    if (!root){
        printf("error loading file!");
        return;
    }

    json_t *values, *arrays, *code, *data;
    int i;

    data = json_object_get(root, "data");
    code = json_object_get(root, "code");
    values = json_object_get(data, "values");
    arrays = json_object_get(data, "arrays");
    

    for (i = 0; i < json_array_size(values); i++){
        json_t *data = json_array_get(values, i);

        json_t *name = json_object_get(data, "name");
        json_t *value = json_object_get(data, "value");
        const char *name_text = json_string_value(name);
        int value_int = json_integer_value(value);

        struct CLLSymbol *sym = cll_lookup_intval(name_text);
        sym->data.value = value_int;
    }

    for (i = 0; i < json_array_size(arrays); i++){
        json_t *data = json_array_get(arrays, i);

        json_t *name = json_object_get(data, "name");
        json_t *fields = json_object_get(data, "fields");
        const char *name_text = json_string_value(name);

        struct CLLSymbol *sym = cll_lookup_array(name_text, 2000);
        int j = 0;
        for (j = 0; j < json_array_size(fields); j++){
            json_t *field = json_array_get(fields, j);
    
            json_t *position = json_object_get(field, "position");
            json_t *value = json_object_get(field, "value");

            int position_int = json_integer_value(position);
            int value_int = json_integer_value(value);

            sym->data.array.array[position_int] = value_int;
        }
    }

    const char *code_string = json_string_value(code);
    yy_scan_string(code_string);
    json_decref(root);  // free pointer

    printf("running code:\n");
    printf("/****************************************/\n");
    printf("%s", code_string);
    printf("/****************************************/\n");
}


void cll_json_final(const char *output_filename){
    json_t *data, *root, *arrays, *values;

    data = json_object();
    root = json_object();
    arrays = json_array();

    int i, j, size = 10;
    const char *arrays_to_print[10] = {"contract.storage", "tx.data"};

    for (i = 0; i < size; i++){
    	if (arrays_to_print[i] == NULL){
    		break;
    	}
    	struct CLLSymbol *symbol = cll_lookup_array(arrays_to_print[i], 0);
    	json_t *json_symbol = json_object();
    	json_t *name_value = json_string(arrays_to_print[i]);
    	
        json_object_set(json_symbol, "name", name_value);
        json_decref(name_value);

    	json_t *fields = json_array();
    	for (j = 0; j < symbol->data.array.size; j++){
    		int value = symbol->data.array.array[j];
    		if (value != 0){
    			json_t *node = json_object();
    			json_t *position = json_integer(j);
    			json_t *value_json = json_integer(value);

    			json_object_set(node, "position", position);
                        json_decref(position);

    			json_object_set(node, "value", value_json);
                        json_decref(value_json);

    			json_array_append(fields, node);
                        json_decref(node);
    		}
    	}
    	json_object_set(json_symbol, "fields", fields);
        json_decref(fields);

    	json_array_append(arrays, json_symbol);
        json_decref(json_symbol);
    }

    json_object_set(root, "arrays", arrays);
    json_decref(arrays);

    json_object_set(data, "data", root);
    json_decref(root);

    json_dump_file(data, output_filename, 0);
    json_decref(data);
}








































