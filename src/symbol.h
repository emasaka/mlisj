#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdbool.h>
#include "mempool.h"

typedef struct {
    char **symbol_table;
    size_t symbol_table_used;

    mempool_t *mempool;
} symbol_pool_t;

extern char *symbol_table_lookup(symbol_pool_t *, char *);
extern char *str2symbol(symbol_pool_t *, char *, bool);

extern void end_symbol(symbol_pool_t *);
extern symbol_pool_t *init_symbol(mempool_t *);

#endif /* _SYMBOL_H */
