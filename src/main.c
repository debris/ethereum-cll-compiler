#include <stdio.h>
#include <stdlib.h>
#include "../include/cll_ast.h"
#include "../include/cll_compiler.h"

// reference: https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-CLL

int yyparse();
void cll_setup();

int main(){
    cll_setup();
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

void cll_setup(){
    struct CLLSymbol *cll_tx_datan          = cll_lookup_intval("tx.datan"); 
    struct CLLSymbol *cll_tx_value          = cll_lookup_intval("tx.value"); 
    struct CLLSymbol *cll_tx_sender         = cll_lookup_intval("tx.sender");
    struct CLLSymbol *cll_contract_address  = cll_lookup_intval("contract.address"); 
    struct CLLSymbol *cll_block_number      = cll_lookup_intval("block.number"); 
    struct CLLSymbol *cll_block_difficulty  = cll_lookup_intval("block.difficulty"); 
    struct CLLSymbol *cll_block_timestamp   = cll_lookup_intval("block.timestamp");
    struct CLLSymbol *cll_block_parenthash  = cll_lookup_intval("block.parenthash");
    struct CLLSymbol *cll_block_basefee     = cll_lookup_intval("block.basefee");


    cll_tx_datan->data.value                = 100;
    cll_tx_value->data.value                = 100;
    cll_tx_sender->data.value               = 100;
    cll_contract_address->data.value        = 100;
    cll_block_number->data.value            = 100;
    cll_block_difficulty->data.value        = 100;
    cll_block_timestamp->data.value         = 100;
    cll_block_parenthash->data.value        = 100;
    cll_block_basefee->data.value           = 100;

    struct CLLSymbol *cll_tx_data           = cll_lookup_array("tx.data", 2000);
    struct CLLSymbol *cll_contract_storage  = cll_lookup_array("contract.storage", 2000);
}









