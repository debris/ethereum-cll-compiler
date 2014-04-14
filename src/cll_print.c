#include <stdio.h>
#include "../include/cll_print.h"
#include "../include/cll_ast.h"

void cll_print_symbol_array(struct CLLSymbol *symbol);

void cll_print_symbol(struct CLLSymbol *symbol){
    printf("cll >> ");
    switch (symbol->symboltype) {
        case CLLSymbolInt:
            printf("%d\n", symbol->data.value);
            break;
        case CLLSymbolArray:
            cll_print_symbol_array(symbol);
            break;
        case CLLSymbolStop:
            printf("stopped at line: %d\n", symbol->data.value);
            break;
        case CLLSymbolSend:
            printf("send to address: %d value: %d gas: %d\n", symbol->data.trans.trans->address, symbol->data.trans.trans->value, symbol->data.trans.trans->gas);
            break;
    }
}

void cll_print_symbol_array(struct CLLSymbol *symbol){
    int i;
    printf("array(%d): ", symbol->data.array.size);
    for (i = 0; i < symbol->data.array.size; ++i) {
        if (symbol->data.array.array[i] != 0){
            printf("[%d] = %d, ", i, symbol->data.array.array[i]);
        }
    }
    printf("\n");
}

void cll_print_final(){
    int i, size = 10;
    char *arrays_to_print[10] = {"contract.storage", "tx.data"};

    printf("RESULT: \n");
    for (i = 0; i < size; i++){
        if (arrays_to_print[i]){
            struct CLLSymbol *symbol = cll_lookup_array(arrays_to_print[i], 0);
            printf("%s: ", arrays_to_print[i]);
            cll_print_symbol_array(symbol);
        }
    }
}



