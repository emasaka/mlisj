#ifndef _READER_H
#define _READER_H

#include "lispobject.h"
#include "lispenv.h"

extern Lisp_Object reader(const char *, lispenv_t *);

#endif /* _READER_H */
