#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdbool.h>
#include "mempool.h"

typedef struct {
    char **symbol_table;
    int symbol_table_used;

    mempool_t *mempool;
} symbol_pool_t;

extern char *str2symbol(symbol_pool_t *, char *, bool);

extern void end_symbol(symbol_pool_t *);
extern symbol_pool_t *init_symbol(mempool_t *);

/* reserved symbols */
extern char * Symbol_minus;
extern char * Symbol_quote;

#endif /* _SYMBOL_H */
