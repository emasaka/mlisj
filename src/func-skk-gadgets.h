/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _FUNC_SKK_GADGETS_H
#define _FUNC_SKK_GADGETS_H

#include "lispenv.h"

extern int register_func_skk_gadgets(lispenv_t *);
extern Lisp_Object skk_relative_date_1(int, int, int, lispenv_t *);

/* export just for tests*/
extern int gengo_to_ad_1(char *, int);
extern Lisp_Object ad_to_gengo_1(lispenv_t *, int, bool, int, int);
extern Lisp_Object split_time_string(char *, lispenv_t *);

#endif /* _FUNC_SKK_GADGETS_H */
