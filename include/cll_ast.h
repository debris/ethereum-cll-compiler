#pragma once

struct CLLSymbol {
    enum { CLLSymbolInt, CLLSymbolArray, CLLSymbolStop } symboltype;  
    
    char *name;
    union {
        int value;
        struct {
            int size;
            int *array;
        } array;
    } data;
};

#define NHASH 9997
struct CLLSymbol symtab[NHASH];

struct CLLSymbol *cll_lookup_intval(const char *);
struct CLLSymbol *cll_lookup_array(const char *, int size);

struct CLLSymlist {
    struct CLLSymbol *sym;
    struct CLLSymlist *next;
};

struct CLLNode{
    enum {
        CLLNodeAst,     // expressions
        CLLNodeSymbol,
        CLLNodeInt,
        CLLNodeSymbolAsgn,
        CLLNodeStmts,
        CLLNodeArrayAccess,
        CLLNodeArrayAsgn,
        CLLNodeIf,
        CLLNodeWhile,
        CLLNodeStop
 
        // TODO signs
    } nodetype;
    //int nodetype;
    union {
        struct {
            int op;
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

        struct {
            int count;
            int size;
            struct CLLNode **stmts;
        } stmts;

        struct {
            struct CLLNode *position;
            struct CLLSymbol *symbol;
        } array_access;

        struct {
            struct CLLNode *position;
            struct CLLSymbol *symbol;
            struct CLLNode *v;
        } array_asgn;
    } data;
};

struct CLLNode *cll_newast(int op, struct CLLNode *l, struct CLLNode *r);
struct CLLNode *cll_newcmp(int op, struct CLLNode *l, struct CLLNode *r);
struct CLLNode *cll_newif(struct CLLNode *cond, struct CLLNode *tl, struct CLLNode *el);
struct CLLNode *cll_newwhile(struct CLLNode *cond, struct CLLNode *tl);
struct CLLNode *cll_newref(struct CLLSymbol *s);
struct CLLNode *cll_newintval(int i);
struct CLLNode *cll_newasgn(struct CLLSymbol *s, struct CLLNode *v);
struct CLLNode *cll_newstmts();
struct CLLNode *cll_addstmt(struct CLLNode *stmts, struct CLLNode *newstmt);
struct CLLNode *cll_newarray_access(struct CLLSymbol *s, struct CLLNode *position);
struct CLLNode *cll_newarray_asgn(struct CLLSymbol *s, struct CLLNode *position, struct CLLNode *v);
struct CLLNode *cll_newstop(int i);


struct CLLSymbol eval(struct CLLNode *);
void treefree(struct CLLNode *);


