#include <stdio.h>
#include "../include/cll_print.h"
#include "../include/cll_ast.h"

void cll_print_symbol_array(struct CLLSymbol *symbol);

void cll_print_symbol(struct CLLSymbol *symbol){
    switch (symbol->symboltype) {
        case CLLSymbolInt:
            printf("cll >> %d\n", symbol->data.value);
            break;
        case CLLSymbolArray:
            cll_print_symbol_array(symbol);
            break;
        case CLLSymbolStop:
            printf("cll >> stop\n");
            break;
    }
}

void cll_print_symbol_array(struct CLLSymbol *symbol){
    int i;
    printf("cll >> array(%d): ", symbol->data.array.size);
    for (i = 0; i < symbol->data.array.size; ++i) {
        if (symbol->data.array.array[i] != 0){
            printf("[%d] = %d, ", i, symbol->data.array.array[i]);
        }
    }
    printf("\n");
}






