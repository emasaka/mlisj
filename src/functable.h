#ifndef _FUNCTABLE_H
#define _FUNCTABLE_H

#include <stdbool.h>

typedef Lisp_Object (*cfunc_t)();

typedef struct {
    char *symbol;
    cfunc_t cfunc;
} cfunc_entry;

typedef struct {
    cfunc_entry *func_table;
    int func_table_used;

    symbol_pool_t *symbol_pool;
} func_pool_t;

extern int add_func(func_pool_t *, char *, cfunc_t);
extern int add_func_from_cstr(func_pool_t *, char *, cfunc_t, bool);
extern cfunc_t get_func(func_pool_t *, char *);

extern func_pool_t *init_func_table(symbol_pool_t *);
extern void end_func_table(func_pool_t *);

#endif /* _FUNCTABLE_H */
