/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _FUNC_SIMPLE_H
#define _FUNC_SIMPLE_H

#include "lispenv.h"

extern int register_func_simple(lispenv_t *);

/* just for tests*/
extern Lisp_Object get_pwd_str(lispenv_t *);

#endif /* _FUNC_SIMPLE_H */
