#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include <stddef.h>
#include "lispobject.h"

/* array node */
typedef struct _NArray {
    size_t size;
    Lisp_Object *data;
} NArray;

extern Lisp_Object *new_lispobject_array(size_t);
extern char *new_string_area(size_t);
extern char *copy_to_string_area(char *);
extern double *cdouble2float(double);
extern NArray *new_narray(size_t);

extern int init_mempool(void);

#endif /* _MEMPOOL_H */
