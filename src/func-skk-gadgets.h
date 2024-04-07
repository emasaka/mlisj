#ifndef _FUNC_SKK_GADGETS_H
#define _FUNC_SKK_GADGETS_H

#include "lispenv.h"

extern int register_func_skk_gadgets(lispenv_t *);

/* just for tests*/
extern int gengo_to_ad_1(char *, int);
extern Lisp_Object ad_to_gengo_1(lispenv_t *, int, bool, int, int);

#endif /* _FUNC_SKK_GADGETS_H */
