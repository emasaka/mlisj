#ifndef _LISPENV_H
#define _LISPENV_H

#include "lispobject.h"
#include "mempool.h"
#include "symbol.h"
#include "variable.h"
#include "functable.h"

typedef struct {
    mempool_t *mempool;
    symbol_pool_t *symbol_pool;
    variable_pool_t *variable_pool;
    func_pool_t *func_pool;

    /* reserved symbols */
    char * Symbol_quote;
    char * Symbol_lambda;
} lispenv_t;

extern lispenv_t *init_lispenv(void);
extern void end_lispenv(lispenv_t *);

#endif /* _LISPENV_H */
