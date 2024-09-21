/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _VARIABLE_H
#define _VARIABLE_H

#include "lispobject.h"
#include "symbol.h"

#define VARIABLE_STACK_SIZE 32

typedef struct {
    const char *symbol;
    Lisp_Object value;
} variable_binding;

typedef struct {
    variable_binding *variable_stack;
    size_t variable_stack_used;

    symbol_pool_t *symbol_pool;
} variable_pool_t;

extern int set_variable(variable_pool_t *, const char *, Lisp_Object);
extern int set_variable_from_cstr(variable_pool_t *, char *, Lisp_Object, bool);
extern Lisp_Object get_variable(const variable_pool_t *, const char *);
extern int save_variable_status(variable_pool_t *);
extern void restore_variable_status(variable_pool_t *, int);

extern variable_pool_t *init_variables(symbol_pool_t *);
extern void end_variables(variable_pool_t *);

#endif /* _VARIABLE_H */
