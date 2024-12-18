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

static unsigned long str_hash_dbj2(const char *str) {
    unsigned long hash = 5381;
    const char *p = str;
    while (*p) {
        hash = ((hash << 5) + hash) + *p++;
    }
    return hash % SYMBOL_TABLE_SIZE;
}

static int hash_table_search(char **table, const char *str) {
    unsigned long hash = str_hash_dbj2(str);
    size_t i = (size_t)hash;
    do {
        if ((table[i] == NULL) || (strcmp(str, table[i]) == 0)) {
            return (int)i;
        }
        if (++i == SYMBOL_TABLE_SIZE) {
            i = 0;
        }
    } while (i != hash);
    /* something wrong */
    return -1;
}

char *symbol_table_lookup(const symbol_pool_t *sp, const char *str) {
    int i = hash_table_search(sp->symbol_table, str);
    if (i == -1) {
        return NULL;
    }
    return sp->symbol_table[i];
}

char *str2symbol(symbol_pool_t *sp, char *str, bool copy_p) {
    if (sp->symbol_table_used == SYMBOL_TABLE_SIZE) {
        /* table is full */
        return NULL;
    }

    int i = hash_table_search(sp->symbol_table, str);
    if (i == -1) {
        /* error */
        return NULL;
    } else if (sp->symbol_table[i] != NULL) {
        /* already existed */
        return sp->symbol_table[i];
    }

    /* add a new symbol */
    char *newstr;
    if (copy_p) {
        newstr =  copy_to_string_area(sp->mempool, str);
        if (newstr == NULL) { return NULL; }
    } else {
        newstr = str;
    }
    sp->symbol_table[i] = newstr;
    sp->symbol_table_used++;
    return newstr;
}

/*
  MARK: initialize and cleanup
 */

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

    char **symbol_table = calloc(SYMBOL_TABLE_SIZE, sizeof(char *));
    if (symbol_table == NULL) {
        free(symbol_pool);
        return NULL;
    }
    symbol_pool->symbol_table = symbol_table;
    symbol_pool->symbol_table_used = 0;
#if !(defined(__x86_64__) || defined(__i386__) || \
      defined(__aarch64__) || defined(__arm__) )
    /* This is not necessary for most CPU architectures */
    for (size_t i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        symbol_table[i] = NULL;
    }
#endif

    symbol_pool->mempool = mempool;

    return symbol_pool;
}
