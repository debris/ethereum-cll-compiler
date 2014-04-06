#include "../include/cll_ast.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


static unsigned cll_symhash(char *sym){
    unsigned int hash = 0;
    unsigned int c = 0;
    while (*sym){
        c = *sym++;
        hash = hash * 9 ^ c;
    }
    return hash;
}

struct CLLSymbol *cll_lookup(char *sym){
   struct CLLSymbol *sp = &symtab[cll_symhash(sym)%NHASH];
   int scount = NHASH;
   while (--scount >= 0){
        if (sp->name && !(strcmp(sp->name, sym))){
            return sp;
        }

        if (!sp->name){
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }
        if (++sp >= symtab+NHASH) sp = symtab;
   }
    yyerror("symbol table overflow\n");
    abort();
    return NULL;
}

struct CLLNode *cll_alloc_node(){
    struct CLLNode *node = malloc(sizeof(struct CLLNode));
    if (!node){
        yyerror("out of space");
        exit(0);
    }
    return node;
}

struct CLLNode *cll_newast(int nodetype, struct CLLNode *l, struct CLLNode *r){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = nodetype;
    a->data.ast.l = l;
    a->data.ast.r = r;
    return a;
}

struct CLLNode *cll_newflow(int nodetype, struct CLLNode *cond, struct CLLNode *tl, struct CLLNode *el){
    struct CLLNode *f = cll_alloc_node();
    f->nodetype = nodetype;
    f->data.flow.cond = cond;
    f->data.flow.tl = tl;
    f->data.flow.el = el;
    return f;
}

struct CLLNode *cll_newref(struct CLLSymbol *s){
    struct CLLNode *r = cll_alloc_node();
    r->nodetype = 'N';
    r->data.symbol = s;
    return r;
}

struct CLLNode *cll_newintval(int i){
    struct CLLNode *n = cll_alloc_node();
    n->nodetype = 'K';
    n->data.number = i;
    return n;
}

struct CLLNode *cll_newasgn(struct CLLSymbol *s, struct CLLNode *v){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = '=';
    a->data.symasgn.s = s;
    a->data.symasgn.v = v;
    return a;
}

int eval(struct CLLNode *a){
    int v = 0;
    if (!a) {
        yyerror("internal error, null eval");
        return 0;
    }

    switch (a->nodetype) {
        case 'K': v = a->data.number; break;
        case 'N': v = a->data.symbol->value; break;

        case '=': v = a->data.symasgn.s->value = eval(a->data.symasgn.v); break;

        case '+': v = eval(a->data.ast.l) + eval(a->data.ast.r); break;
        case '-': v = eval(a->data.ast.l) - eval(a->data.ast.r); break;
        case '*': v = eval(a->data.ast.l) * eval(a->data.ast.r); break;
        case '/': v = eval(a->data.ast.l) / eval(a->data.ast.r); break;
        case 'M': v = -eval(a->data.ast.l); break;

        case 'I': /* TODO */ break;
        case 'W': /* TODO */ break;

        default: printf("internal erro: bad node %c\n", a->nodetype);
    }
    return v;
}


void treefree(struct CLLNode *a){
    switch (a->nodetype){
            /* two substrees */
        case '+':
        case '-':
        case '*':
        case '/':
            treefree(a->data.ast.r);
            
            /* one substree */
        case 'M':
            treefree(a->data.ast.l);
            break;

        case 'K':
        case 'N': 
            break;

        case '=':
            free(a->data.symasgn.v);
            break;

        case 'I':
        case 'W':
            /* TODO */
            break;
        default: printf("internal error: free bad node %c\n", a->nodetype);
    }
    free(a);
}






















