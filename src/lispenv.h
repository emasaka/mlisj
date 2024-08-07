/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _LISPENV_H
#define _LISPENV_H

#include <time.h>
#include "lispobject.h"
#include "mempool.h"
#include "symbol.h"
#include "variable.h"
#include "functable.h"

typedef struct _lispenv {
    mempool_t *mempool;
    symbol_pool_t *symbol_pool;
    variable_pool_t *variable_pool;
    func_pool_t *func_pool;

    /* reserved symbols */
    char * Symbol_quote;
    char * Symbol_lambda;

    /* base of skk-num-list */
    char **skk_num_list;
    /* skk-henkan-key */
    const char *skk_henkan_key;

    /* function to get current time */
    void (*current_time_func)(struct tm *);
} lispenv_t;

extern lispenv_t *init_lispenv(void);
extern void end_lispenv(lispenv_t *);
extern void register_skk_num_list(lispenv_t *, char **);
extern void register_skk_henkan_key(lispenv_t *, const char *);

#endif /* _LISPENV_H */
