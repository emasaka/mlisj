/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include <stddef.h>
#include "lispobject.h"

#define LISPOBJECT_POOL_SIZE 1024
#define STRING_POOL_SIZE 1024
#define FLOAT_POOL_SIZE 32
#define NARRAY_NODE_POOL_SIZE 64

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

#if !DOUBLE_IMMEDIATE
    double *float_pool;
    size_t float_pool_used;
#endif

    NArray *narray_node_pool;
    size_t narray_node_pool_used;
} mempool_t;

extern Lisp_Object *new_lispobject_array(mempool_t *, size_t);
extern char *new_string_area(mempool_t *, size_t);
extern char *copy_to_string_area(mempool_t *, const char *);
#if !DOUBLE_IMMEDIATE
extern double *cdouble2float(mempool_t *, double);
#endif
extern NArray *new_narray(mempool_t *, size_t);
extern NArray *cdr_narray(mempool_t *, NArray *);

extern void end_mempool(mempool_t *);
extern mempool_t *init_mempool(void);

#endif /* _MEMPOOL_H */
