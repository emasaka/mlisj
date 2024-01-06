#include <string.h>
#include "lispobject.h"
#include "symbol.h"
#include "functable.h"

typedef int (*cfunc_t)();

/* internal type */
typedef struct {
    char *symbol;
    cfunc_t cfunc;
} cfunc_entry;

#define FUNC_TABLE_SIZE 256

static cfunc_entry func_table[FUNC_TABLE_SIZE];

static int func_table_used = 0;

int add_func(char *sym, cfunc_t func) {
    /* duplicate check omitted*/
    if (func_table_used < FUNC_TABLE_SIZE) {
        func_table[func_table_used++] = (cfunc_entry){ .symbol = sym, .cfunc = func };
        return 0;
    } else {
        return -1;
    }
}

int add_func_from_cstr(char *str, cfunc_t func) {
    char *sym = str2symbol(str, true);
    if (sym == NULL) { return -1; }
    return add_func(sym, func);
}

cfunc_t get_func(char *sym) {
    /* linear search, for simplicity */
    for (int i = 0; i < func_table_used; i++) {
        if (func_table[i].symbol == sym) {
            return func_table[i].cfunc;
        }
    }
    // not found
    return NULL;
}

int init_func_table(void) {
    func_table_used = 0;
    // return the value of success, just in case
    return 0;
}
