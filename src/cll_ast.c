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
                sp->data.array.size = size;
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

struct CLLNode *cll_newstop(int i){
    struct CLLNode *a = cll_alloc_node();
    a->nodetype = CLLNodeStop;
    a->data.number = i;
    return a;
}


struct CLLSymbol eval(struct CLLNode *a){
    int i;
    struct CLLSymbol result;
    if (!a) {
        yyerror("internal error, null eval");
        return result; // null result
    }

    //printf("o");
    switch (a->nodetype) {
        case CLLNodeAst:
            {   
                struct CLLSymbol eval_l = eval(a->data.ast.l);
                struct CLLSymbol eval_r;
                if (a->data.ast.r){
                    eval_r = eval(a->data.ast.r);
                }

                result.symboltype = CLLSymbolInt;
                switch (a->data.ast.op) {
                    case '+':
                        result.data.value = eval_l.data.value + eval_r.data.value;
                        break;
                    case '-':
                        result.data.value = eval_l.data.value - eval_r.data.value;
                        break;
                    case '*':
                        result.data.value = eval_l.data.value * eval_r.data.value;
                        break;
                    case '/':
                        result.data.value = eval_l.data.value / eval_r.data.value;
                        break;
                    case '%':
                        result.data.value = eval_l.data.value % eval_r.data.value;
                        break;
                    case '^':
                        result.data.value = eval_l.data.value ^ eval_r.data.value;
                        break;
                    case 'M':
                        result.data.value = -eval_l.data.value;
                        break;
                    case '1': 
                        result.data.value = eval_l.data.value > eval_r.data.value ? 1 : 0;
                        break;
                    case '2':
                        result.data.value = eval_l.data.value < eval_r.data.value ? 1 : 0;
                        break;
                    case '3':
                        result.data.value = eval_l.data.value != eval_r.data.value ? 1 : 0;
                        break;
                    case '4':
                        result.data.value = eval_l.data.value == eval_r.data.value ? 1 : 0;
                        break;
                    case '5':
                        result.data.value = eval_l.data.value >= eval_r.data.value ? 1 : 0;
                        break;
                    case '6':
                        result.data.value = eval_l.data.value <= eval_r.data.value ? 1 : 0;
                        break;
                    case '7':
                        result.data.value = eval_l.data.value || eval_r.data.value ? 1 : 0;
                        break;
                    case '8':
                        result.data.value = eval_l.data.value && eval_r.data.value ? 1 : 0;
                        break;
                    default: printf("internal error, no matching operator for ast %c\n", a->data.ast.op);
                }
            }   break;

        case CLLNodeSymbol:
            {
                result.symboltype = a->data.symbol->symboltype;
                switch (a->data.symbol->symboltype) {
                    case CLLSymbolInt: 
                        result.data.value = a->data.symbol->data.value;
                        break;
                    case CLLSymbolArray:
                        result.data.array.size = a->data.symbol->data.array.size;
                        result.data.array.array = a->data.symbol->data.array.array;
                        //printf("debug :%d : %d", result.data.array.size, a->data.symbol->data.array.size);
                        break;
                    case CLLSymbolStop:
                        break;
                }
            }   break;

        case CLLNodeInt:
            result.symboltype = CLLSymbolInt;
            result.data.value = a->data.number; 
            break;
        case CLLNodeSymbolAsgn:
            {
                struct CLLSymbol eval_v = eval(a->data.symasgn.v);
                result.symboltype = a->data.symasgn.s->symboltype;
                switch (a->data.symasgn.s->symboltype) {
                    case CLLSymbolInt:
                        result.data.value = a->data.symasgn.s->data.value = eval_v.data.value;
                        break;
                    case CLLSymbolArray:
                        result.data.array.size = a->data.symasgn.s->data.array.size = eval_v.data.array.size;
                        result.data.array.array = a->data.symasgn.s->data.array.array = eval_v.data.array.array;
                        break;
                    case CLLSymbolStop: 
                        break;
                }
            }   break;
        case CLLNodeStmts:
            {
                int type = -1;
                struct CLLSymbol stmt_symbol;
                for (i = 0; i < a->data.stmts.count && type != CLLSymbolStop; ++i){
                    stmt_symbol = eval(a->data.stmts.stmts[i]);
                    type = stmt_symbol.symboltype;
                }
                if (type == CLLSymbolStop){
                    result.symboltype = CLLSymbolStop;
                    result.data.value = stmt_symbol.data.value;
                } else {
                    result.symboltype = CLLSymbolInt;
                    result.data.value = 1;
                }
            }
            break;

        case CLLNodeArrayAccess:
            {
                struct CLLSymbol eval_p = eval(a->data.array_access.position);
                result.symboltype = CLLSymbolInt;
                result.data.value = a->data.array_access.symbol->data.array.array[eval_p.data.value];
            }
            break;
        case CLLNodeArrayAsgn:
            {
                struct CLLSymbol eval_p = eval(a->data.array_asgn.position);
                struct CLLSymbol eval_v = eval(a->data.array_asgn.v);
                result.symboltype = CLLSymbolInt;
                result.data.value = a->data.array_asgn.symbol->data.array.array[eval_p.data.value] = eval_v.data.value;
            }
            break;
        case CLLNodeIf:
            {
                struct CLLSymbol eval_cond = eval(a->data.flow.cond);
                if (eval_cond.data.value != 0){
                    if (a->data.flow.tl){
                        struct CLLSymbol eval_tl = eval(a->data.flow.tl);
                        result = eval_tl;
                    } else {
                        result.symboltype = CLLSymbolInt;
                        result.data.value = 0;
                    }
                } else if (a->data.flow.el) {
                    struct CLLSymbol eval_el = eval(a->data.flow.el);
                    result = eval_el;
                } else {
                    result.symboltype = CLLSymbolInt;
                    result.data.value = 0;
                }
            } break;
        case CLLNodeWhile:
            {   
                if (a->data.flow.tl){
                    int type = -1;
                    struct CLLSymbol stmt_symbol;
                    while (eval(a->data.flow.cond).data.value != 0 && type != CLLSymbolStop) {
                        stmt_symbol = eval(a->data.flow.tl);
                        type = stmt_symbol.symboltype;
                    }
                    if (type == CLLSymbolStop) {
                        result.symboltype = CLLSymbolStop;
                        result.data.value = stmt_symbol.data.value;
                    } else {
                        result.symboltype = CLLSymbolInt;
                        result.data.value = 1;
                    }
                }
            } break;

        case CLLNodeStop:
            result.symboltype = CLLSymbolStop;
            result.data.value = a->data.number;
            break;


        default: printf("internal erro: bad node %d\n", a->nodetype);
    }
    return result;
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























