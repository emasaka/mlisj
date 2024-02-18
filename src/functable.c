#include <stdlib.h>
#include <string.h>
#include "lispobject.h"
#include "symbol.h"
#include "functable.h"

#define FUNC_TABLE_SIZE 256

int add_func(func_pool_t *fp, char *sym, cfunc_t func) {
    /* duplicate check omitted*/
    if (fp->func_table_used < FUNC_TABLE_SIZE) {
        fp->func_table[fp->func_table_used++] = (cfunc_entry){ .symbol = sym, .cfunc = func };
        return 0;
    } else {
        return -1;
    }
}

int add_func_from_cstr(func_pool_t *fp, char *str, cfunc_t func, bool copy_p) {
    char *sym = str2symbol(fp->symbol_pool, str, copy_p);
    if (sym == NULL) { return -1; }
    return add_func(fp, sym, func);
}

cfunc_t get_func(func_pool_t *fp, char *sym) {
    /* linear search, for simplicity */
    for (int i = 0; i < fp->func_table_used; i++) {
        if (fp->func_table[i].symbol == sym) {
            return fp->func_table[i].cfunc;
        }
    }
    // not found
    return NULL;
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

    cfunc_entry *func_table = malloc(sizeof(cfunc_entry) * FUNC_TABLE_SIZE);
    if (func_table == NULL) {
        free(func_pool);
        return NULL;
    }
    func_pool->func_table = func_table;
    func_pool->func_table_used = 0;

    func_pool->symbol_pool = symbol_pool;

    return func_pool;
}
