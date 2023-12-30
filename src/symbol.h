#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdbool.h>

extern char *str2symbol(char *, bool);

extern void init_symbol(void);

/* reserved symbols */
extern char * Symbol_minus;
extern char * Symbol_quote;

#endif /* _SYMBOL_H */
