/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _EVAL_H
#define _EVAL_H

#include "lispobject.h"
#include "lispenv.h"

extern Lisp_Object call_lambda(Lisp_Object, NArray *, lispenv_t *);
extern Lisp_Object eval_expr(Lisp_Object, lispenv_t *);

#endif /* _EVAL_H */
