#include "../include/cll_ast.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


static unsigned cll_symhash(const char *sym){
    unsigned int hash = 0;
    unsigned int c = 0;
    while (*sym){
        c = *sym++;
        hash = hash * 9 ^ c;
    }
    return hash;
}

struct CLLSymbol *cll_lookup(int symboltype, const char *sym, int size){
   struct CLLSymbol *sp = &symtab[cll_symhash(sym)%NHASH];
   int scount = NHASH;
   while (--scount >= 0){
        if (sp->name && !(strcmp(sp->name, sym))){
            // free array if symboltype != sp->symboltype
            return sp;
        }

        if (!sp->name){
            sp->name = strdup(sym);
            if (symboltype == CLLSymbolInt){
                sp->data.value = 0;
            } else {
                sp->data.array.array = malloc(sizeof(int) * size); 
            }
            sp->symboltype = symboltype;
            return sp;
        }
        if (++sp >= symtab+NHASH) sp = symtab;
   }
    yyerror("symbol table overflow\n");
    abort();
    return NULL;
}

struct CLLSymbol *cll_lookup_intval(const char *sym){
   return cll_lookup(CLLSymbolInt, sym, 0); 
}

struct CLLSymbol *cll_lookup_array(const char *sym, int size){
    return cll_lookup(CLLSymbolArray, sym, size);
}


struct CLLNode *cll_alloc_node(){
    struct CLLNode *node = malloc(sizeof(struct CLLNode));
    if (!node){
        yyerror("out of space");
        exit(0);
    }
    return node;
}

struct CLLNode *cll_newast(int op, struct CLLNode *l, struct CLLNode *r){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = CLLNodeAst;
    a->data.ast.op = op;
    a->data.ast.l = l;
    a->data.ast.r = r;
    return a;
}

struct CLLNode *cll_newcmp(int op, struct CLLNode *l, struct CLLNode *r){
    struct CLLNode *c = cll_alloc_node();
    c->nodetype = CLLNodeAst;
    c->data.ast.op = '0' + op;
    c->data.ast.l = l;
    c->data.ast.r = r;
    return c;
}

struct CLLNode *cll_newif(struct CLLNode *cond, struct CLLNode *tl, struct CLLNode *el){
    struct CLLNode *f = cll_alloc_node();
    f->nodetype = CLLNodeIf;
    f->data.flow.cond = cond;
    f->data.flow.tl = tl;
    f->data.flow.el = el;
    return f;
}

struct CLLNode *cll_newwhile(struct CLLNode *cond, struct CLLNode *tl){
    struct CLLNode *f = cll_alloc_node();
    f->nodetype = CLLNodeWhile;
    f->data.flow.cond = cond;
    f->data.flow.tl = tl;
    f->data.flow.el = NULL;
    return f;
}

struct CLLNode *cll_newref(struct CLLSymbol *s){
    struct CLLNode *r = cll_alloc_node();
    r->nodetype = CLLNodeSymbol;
    r->data.symbol = s;
    return r;
}

struct CLLNode *cll_newintval(int i){
    struct CLLNode *n = cll_alloc_node();
    n->nodetype = CLLNodeInt;
    n->data.number = i;
    return n;
}

struct CLLNode *cll_newasgn(struct CLLSymbol *s, struct CLLNode *v){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = CLLNodeSymbolAsgn; 
    a->data.symasgn.s = s;
    a->data.symasgn.v = v;
    return a;
}

struct CLLNode *cll_newstmts(){
    struct CLLNode *s = cll_alloc_node();
    s->nodetype = CLLNodeStmts;
    s->data.stmts.count = 0;
    s->data.stmts.size = 4;
    s->data.stmts.stmts = calloc(sizeof(struct CLLNode*), s->data.stmts.size);
    return s;
}

struct CLLNode *cll_addstmt(struct CLLNode *stmts, struct CLLNode *newstmt){
    if (stmts->data.stmts.count == stmts->data.stmts.size){
        stmts->data.stmts.size *= 2;
        stmts->data.stmts.stmts = realloc(stmts->data.stmts.stmts, stmts->data.stmts.size * sizeof(struct CLLNode *));
    }
    stmts->data.stmts.stmts[stmts->data.stmts.count] = newstmt;
    ++stmts->data.stmts.count;
    return stmts;
}

struct CLLNode *cll_newarray_access(struct CLLSymbol *s, struct CLLNode *position){
   struct CLLNode *a = cll_alloc_node();
   a->nodetype = CLLNodeArrayAccess;
   a->data.array_access.symbol = s;
   a->data.array_access.position = position;
   return a;
}

struct CLLNode *cll_newarray_asgn(struct CLLSymbol *s, struct CLLNode *position, struct CLLNode *v){
    struct CLLNode *a =cll_alloc_node();
    a->nodetype = CLLNodeArrayAsgn;
    a->data.array_asgn.symbol = s;
    a->data.array_asgn.position = position;
    a->data.array_asgn.v = v;
    return a;
}

struct CLLNode *cll_newstop(){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = CLLNodeStop;
    return a;
}


int eval(struct CLLNode *a){
    int v = 0;
    int i;
    if (!a) {
        yyerror("internal error, null eval");
        return 0;
    }

    //printf("node: %d\n", a->nodetype);
    switch (a->nodetype) {
        case CLLNodeAst:
            {
                switch (a->data.ast.op) {
                    case '+': v = eval(a->data.ast.l) + eval(a->data.ast.r); break;
                    case '-': v = eval(a->data.ast.l) - eval(a->data.ast.r); break;
                    case '*': v = eval(a->data.ast.l) * eval(a->data.ast.r); break;
                    case '/': v = eval(a->data.ast.l) / eval(a->data.ast.r); break;
                    case '%': v = eval(a->data.ast.l) % eval(a->data.ast.r); break;
                    case '^': v = eval(a->data.ast.l) ^ eval(a->data.ast.r); break;
                    case 'M': v = -eval(a->data.ast.l); break;
                    case '1': v = eval(a->data.ast.l) > eval(a->data.ast.r) ? 1 : 0; break;
                    case '2': v = eval(a->data.ast.l) < eval(a->data.ast.r) ? 1 : 0; break;
                    case '3': v = eval(a->data.ast.l) != eval(a->data.ast.r)? 1 : 0; break;
                    case '4': v = eval(a->data.ast.l) == eval(a->data.ast.r)? 1 : 0; break;
                    case '5': v = eval(a->data.ast.l) >= eval(a->data.ast.r)? 1 : 0; break;
                    case '6': v = eval(a->data.ast.l) <= eval(a->data.ast.r)? 1 : 0; break;
                    default: printf("internal error, no matching operator for ast %c\n", a->data.ast.op);
                }
            }   break;

        case CLLNodeSymbol:
            {
                switch (a->data.symbol->symboltype) {
                    case CLLSymbolInt: v = a->data.symbol->data.value; break;
                    case CLLSymbolArray: v = 1; break;
                    case CLLSymbolStop: v = 0; break;
                }
            }   break;

        case CLLNodeInt:
            v = a->data.number; break;
        case CLLNodeSymbolAsgn:
            {
                switch (a->data.symasgn.s->symboltype) {
                    case CLLSymbolInt: v = a->data.symasgn.s->data.value = eval(a->data.symasgn.v); break;
                    case CLLSymbolArray: v = 1; break;
                    case CLLSymbolStop: v = 0; break;
                }
            }   break;
        case CLLNodeStmts:
            for (i = 0; i < a->data.stmts.count; ++i){
                eval(a->data.stmts.stmts[i]);
            }
            v = 1;
            break;

        case CLLNodeArrayAccess:
            v = a->data.array_access.symbol->data.array.array[eval(a->data.array_access.position)]; break;
        case CLLNodeArrayAsgn:
            v = a->data.array_asgn.symbol->data.array.array[eval(a->data.array_access.position)] = eval(a->data.array_asgn.v); break;
        case CLLNodeIf:
            if (eval(a->data.flow.cond) != 0){
                if (a->data.flow.tl){
                    v = eval(a->data.flow.tl);
                } else {
                    v = 0;
                }
            } else if (a->data.flow.el){
                v = eval(a->data.flow.el);
            } else {
                v = 0;
            }
            break;
        case CLLNodeWhile:
            if (a->data.flow.tl){
                while (eval(a->data.flow.cond) != 0){
                    v = eval(a->data.flow.tl);
                }
            }
            break;
        case CLLNodeStop:
            v = 0;
            break;


        default: printf("internal erro: bad node %d\n", a->nodetype);
    }
    return v;
}


void treefree(struct CLLNode *a){
    /* TODO */
    return;
    /*
    switch (a->nodetype){
            // two substrees 
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '%':
        case '1': case '2': case '3':
        case '4': case '5': case '6':
            treefree(a->data.ast.r);
            
            // one substree
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
            // TODO 
            break;
        default: printf("internal error: free bad node %c\n", a->nodetype);
    } */
    free(a);
}























