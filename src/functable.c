/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lispobject.h"
#include "symbol.h"
#include "functable.h"

static size_t funcptr_hash(const char *sym) {
    return (size_t)((uintptr_t)sym % FUNC_TABLE_SIZE);
}

static int func_hash_table_search(cfunc_entry *table, const char *sym) {
    size_t hash = funcptr_hash(sym);
    size_t i = hash;
    do {
        if ((table[i].symbol == NULL) || (table[i].symbol == sym)) {
            return (int)i;
        }
        if (++i == FUNC_TABLE_SIZE) {
            i = 0;
        }
    } while (i != hash);
    /* something wrong */
    return -1;
}

int add_func(func_pool_t *fp, const char *sym, cfunc_t func) {
    if (fp->func_table_used >= FUNC_TABLE_SIZE) {
        return -1;
    }

    int i = func_hash_table_search(fp->func_table, sym);
    if (i == -1) {
        /* error */
        return -1;
    }
    if (fp->func_table[i].symbol == NULL) {
        fp->func_table[i] = (cfunc_entry){ .symbol = sym, .cfunc = func };
        fp->func_table_used++;
        return 0;
    } else {
        /* already existed */
        return -1;
    }
}

int add_func_from_cstr(func_pool_t *fp, char *str, cfunc_t func, bool copy_p) {
    char *sym = str2symbol(fp->symbol_pool, str, copy_p);
    if (sym == NULL) { return -1; }
    return add_func(fp, sym, func);
}

cfunc_t get_func(const func_pool_t *fp, const char *sym) {
    int i = func_hash_table_search(fp->func_table, sym);
    if ((i == -1)  || (fp->func_table[i].symbol == NULL)) {
        return NULL;
    } else {
        return fp->func_table[i].cfunc;
    }
}

void end_func_table(func_pool_t *func_pool) {
    if (func_pool->func_table != NULL) {
        free(func_pool->func_table);
    }
    free(func_pool);
}

func_pool_t *init_func_table(symbol_pool_t *symbol_pool) {
    func_pool_t *func_pool = malloc(sizeof(func_pool_t));
    if (func_pool == NULL) {
        return NULL;
    }

    cfunc_entry *func_table = calloc(FUNC_TABLE_SIZE, sizeof(cfunc_entry));
    if (func_table == NULL) {
        free(func_pool);
        return NULL;
    }
    func_pool->func_table = func_table;
    func_pool->func_table_used = 0;
#if !(defined(__x86_64__) || defined(__i386__) || \
      defined(__aarch64__) || defined(__arm__) )
    /* This is not necessary for most CPU architectures */
    for (size_t i = 0; i < FUNC_TABLE_SIZE; i++) {
        func_table[i].symbol = NULL;
    }
#endif

    func_pool->symbol_pool = symbol_pool;

    return func_pool;
}
