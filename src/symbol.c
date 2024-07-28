/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lispobject.h"
#include "mempool.h"
#include "symbol.h"

/* linear search, for simplicity */
char *symbol_table_lookup(symbol_pool_t *sp, const char *str) {
    for (size_t i = 0; i < sp->symbol_table_used; i++) {
        if (strcmp(str, sp->symbol_table[i]) == 0) {
            return sp->symbol_table[i];
        }
    }
    /* not found */
    return NULL;
}

char *new_symbol(symbol_pool_t *sp, char *str, bool copy_p) {
    if (sp->symbol_table_used == SYMBOL_TABLE_SIZE) {
        return NULL;
    }
    char *newstr;
    if (copy_p) {
        newstr =  copy_to_string_area(sp->mempool, str);
        if (newstr == NULL) { return NULL; }
    } else {
        newstr = str;
    }
    sp->symbol_table[sp->symbol_table_used++] = newstr;
    return newstr;
}

char *str2symbol(symbol_pool_t *sp, char *str, bool copy_p) {
    char *p = symbol_table_lookup(sp, str);
    if (p) {
        /* already existed */
        return p;
    } else {
        return new_symbol(sp, str, copy_p);
    }
}

void end_symbol(symbol_pool_t *symbol_pool) {
    if (symbol_pool->symbol_table != NULL) {
        free(symbol_pool->symbol_table);
    }
    free(symbol_pool);
}

symbol_pool_t *init_symbol(mempool_t *mempool) {
    symbol_pool_t *symbol_pool = malloc(sizeof(symbol_pool_t));
    if (symbol_pool == NULL) {
        return NULL;
    }

    char **symbol_table = malloc(sizeof(char *) * SYMBOL_TABLE_SIZE);
    if (symbol_table == NULL) {
        free(symbol_pool);
        return NULL;
    }
    symbol_pool->symbol_table = symbol_table;
    symbol_pool->symbol_table_used = 0;

    symbol_pool->mempool = mempool;

    return symbol_pool;
}
