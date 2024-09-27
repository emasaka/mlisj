/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _READER_H
#define _READER_H

#include "lispobject.h"
#include "lispenv.h"

#define READER_MAX_NESTING_LEVEL 32

extern Lisp_Object reader(const char *, lispenv_t *);

#endif /* _READER_H */
