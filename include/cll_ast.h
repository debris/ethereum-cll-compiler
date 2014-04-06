#pragma once

struct CLLSymbol {
    char *name;
    int value;
    struct CLLNode *func;
    struct CLLSymlist *syms;
};

#define NHASH 9997
struct CLLSymbol symtab[NHASH];

struct CLLSymbol *cll_lookup(char *);

struct CLLSymlist {
    struct CLLSymbol *sym;
    struct CLLSymlist *next;
};

struct CLLNode{
    int nodetype;
    union {
        struct {
            struct CLLNode *l;
            struct CLLNode *r;
        } ast;

        struct {
            struct CLLNode *cond;
            struct CLLNode *tl;
            struct CLLNode *el;
        } flow; /* if while */

        struct CLLSymbol *symbol;
        int number;

        struct {
            struct CLLSymbol *s;
            struct CLLNode *v;
        } symasgn;

    } data;
};

struct CLLNode *cll_newast(int nodetype, struct CLLNode *l, struct CLLNode *r);
struct CLLNode *cll_newflow(int nodetype, struct CLLNode *cond, struct CLLNode *tl, struct CLLNode *el);
struct CLLNode *cll_newref(struct CLLSymbol *s);
struct CLLNode *cll_newintval(int i);
struct CLLNode *cll_newasgn(struct CLLSymbol *s, struct CLLNode *v);

int eval(struct CLLNode *);
void treefree(struct CLLNode *);


