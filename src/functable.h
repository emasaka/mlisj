/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _FUNCTABLE_H
#define _FUNCTABLE_H

#include <stdbool.h>

#define FUNC_TABLE_SIZE 32

/* prototype declarations of struct */
struct _Narray;
struct _lispenv;

typedef Lisp_Object (*cfunc_t)(struct _NArray *, struct _lispenv *);

typedef struct {
    char *symbol;
    cfunc_t cfunc;
} cfunc_entry;

typedef struct {
    cfunc_entry *func_table;
    size_t func_table_used;

    symbol_pool_t *symbol_pool;
} func_pool_t;

extern int add_func(func_pool_t *, char *, cfunc_t);
extern int add_func_from_cstr(func_pool_t *, char *, cfunc_t, bool);
extern cfunc_t get_func(func_pool_t *, const char *);

extern func_pool_t *init_func_table(symbol_pool_t *);
extern void end_func_table(func_pool_t *);

#endif /* _FUNCTABLE_H */
