#include <string.h>
#include <stdbool.h>
#include "lispobject.h"
#include "mempool.h"

#define SYMBOL_TABLE_SIZE 256

static char *symbol_table[SYMBOL_TABLE_SIZE];

static int symbol_table_used = 0;

/* reserved symbols */
char * Symbol_minus = NULL;
char * Symbol_quote = NULL;

/* linear search, for simplicity */
char *symbol_table_lookup(char *str) {
    for (int i = 0; i < symbol_table_used; i++) {
        if (strcmp(str, symbol_table[i]) == 0) {
            return symbol_table[i];
        }
    }
    /* not found */
    return NULL;
}

char *new_symbol(char*str, bool copy_p) {
    if (symbol_table_used == SYMBOL_TABLE_SIZE) {
        return NULL;
    }
    char *newstr;
    if (copy_p) {
        newstr =  copy_to_string_area(str);
        if (newstr == NULL) { return NULL; }
    } else {
        newstr = str;
    }
    symbol_table[symbol_table_used++] = newstr;
    return newstr;
}

char *str2symbol(char *str, bool copy_p) {
    char *p = symbol_table_lookup(str);
    if (p) {
        /* already existed */
        return p;
    } else {
        return new_symbol(str, copy_p);
    }
}

void init_symbol(void) {
    symbol_table_used = 0;
}
