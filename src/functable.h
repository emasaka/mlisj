#ifndef _FUNCTABLE_H
#define _FUNCTABLE_H

typedef int (*cfunc_t)();

extern int add_func(char *sym, int (*func)());
extern cfunc_t get_func(char *);
extern int init_func_table(void);

#endif /* _FUNCTABLE_H */