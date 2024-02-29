#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include <stddef.h>
#include "lispobject.h"

/* array node */
typedef struct _NArray {
    size_t size;
    Lisp_Object *data;
} NArray;

typedef struct {
    Lisp_Object *lispobject_pool;
    size_t lispobject_pool_used;

    char *string_pool;
    size_t string_pool_used;

    double *float_pool;
    size_t float_pool_used;

    NArray *narray_node_pool;
    size_t narray_node_pool_used;
} mempool_t;

extern Lisp_Object *new_lispobject_array(mempool_t *, size_t);
extern char *new_string_area(mempool_t *, size_t);
extern char *copy_to_string_area(mempool_t *, char *);
extern double *cdouble2float(mempool_t *, double);
extern NArray *new_narray(mempool_t *, size_t);

extern void end_mempool(mempool_t *);
extern mempool_t *init_mempool(void);

#endif /* _MEMPOOL_H */
