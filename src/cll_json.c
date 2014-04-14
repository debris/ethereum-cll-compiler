#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include "../include/cll_json.h"
#include "../include/cll_ast.h"

void yy_scan_string();  // fix this!

char *cll_read_file(const char *filename);

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

    char *plain_text_code = "";
    

    json_t *code_source = json_object_get(code, "source");
    const char *code_source_string = json_string_value(code_source);
    if (!strcmp(code_source_string, "plain_text")){
        json_t *plain_text = json_object_get(code, "plain_text");
        plain_text_code = json_string_value(plain_text);
        yy_scan_string(plain_text_code);
    } else if (!strcmp(code_source_string, "file")){
        json_t *file_name = json_object_get(code, "file");
        const char *plain_file_name = json_string_value(file_name);
        printf("%s ", plain_file_name);
        plain_text_code = cll_read_file(plain_file_name);
        yy_scan_string(plain_text_code);
    } else {
        printf("cannot read source code!\n");
    }



    printf("running code:\n");
    printf("/****************************************/\n");
    printf("%s", plain_text_code);
    printf("/****************************************/\n");

    json_decref(root);  // free pointer
}


void cll_json_final(const char *output_filename){
    json_t *data, *root, *arrays;

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

    struct CLLSymbol *tx = cll_lookup_transactions();
    json_t *tx_json_array = json_array();

    for (i = 0; i < tx->data.trans.current; i++){
        json_t *tx_json = json_object(); 
        json_t *address_json = json_integer(tx->data.trans.trans[i].address);
        json_t *value_json = json_integer(tx->data.trans.trans[i].value);
        json_t *gas_json = json_integer(tx->data.trans.trans[i].gas);

        json_object_set(tx_json, "address", address_json);
        json_object_set(tx_json, "value", value_json);
        json_object_set(tx_json, "gas", gas_json);

        json_array_append(tx_json_array, tx_json);

        json_decref(tx_json);
        json_decref(address_json);
        json_decref(value_json);
        json_decref(gas_json);
    }
    
    json_object_set(root, "transactions", tx_json_array);
    json_decref(tx_json_array);

    struct CLLSymbol *saved_stop = cll_lookup_intval("stop");
    json_t *json_stop_line = json_integer(saved_stop->data.value);
    json_object_set(root, "stop", json_stop_line);
    json_decref(json_stop_line);

    json_object_set(root, "arrays", arrays);
    json_decref(arrays);

    json_object_set(data, "data", root);
    json_decref(root);

    json_dump_file(data, output_filename, 0);
    json_decref(data);
}








































