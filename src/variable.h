#ifndef _VARIABLE_H
#define _VARIABLE_H

#include "lispobject.h"

extern int set_variable(char *, Lisp_Object;
extern Lisp_Object get_variable(char *);
extern int save_variable_status(void);
extern void restore_variable_status(int);
extern int init_variables(void);

#endif /* _VARIABLE_H */
