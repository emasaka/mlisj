/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "lispobject.h"
#include "mempool.h"

/*
  Lisp_Object Pool
 */

/* obtain new lispobject pool area */
Lisp_Object *new_lispobject_array(mempool_t *mp, size_t n) {
    if (mp->lispobject_pool_used + n > LISPOBJECT_POOL_SIZE) {
        return NULL;
    } else {
        Lisp_Object *p =  &(mp->lispobject_pool[mp->lispobject_pool_used]);
        mp->lispobject_pool_used += n;
        return p;
    }
}

/*
  String Pool
 */

/* obtain new string pool area */
char *new_string_area(mempool_t *mp, size_t n) {
    if (mp->string_pool_used + n > STRING_POOL_SIZE) {
        return NULL;
    } else {
        char *p =  &(mp->string_pool[mp->string_pool_used]);
        mp->string_pool_used += n;
        return p;
    }
}

/* copy C string to string pool */
char *copy_to_string_area(mempool_t *mp, const char *str) {
    size_t len = strlen(str);
    char *area = new_string_area(mp, len + 1);
    if (area == NULL) { return NULL; }
    memcpy(area, str, len + 1);
    return area;
}

#if !DOUBLE_IMMEDIATE
/*
  Float Pool
 */

/* obtain new float pool area and copy number */
double *cdouble2float(mempool_t *mp, double f) {
    if (mp->float_pool_used == FLOAT_POOL_SIZE) {
        return NULL;
    } else {
        double *p = &(mp->float_pool[mp->float_pool_used++]);
        *p = f;
        return p;
    }
}
#endif

/*
  Array Node Pool
  (bodies are on lispobject_pool)
 */

/* obtain new narray  */
NArray *new_narray(mempool_t *mp, size_t n) {
    if (mp->narray_node_pool_used == NARRAY_NODE_POOL_SIZE) {
        return NULL;
    }
    Lisp_Object *array;
    if (n > 0) {
        array = new_lispobject_array(mp, n);
        if (array == NULL) { return NULL; }
    } else {
        array = NULL;
    }
    NArray *node = &(mp->narray_node_pool[mp->narray_node_pool_used++]);
    node->size = n;
    node->data = array;
    return node;
}

/* cdr of narray (body is shared) */
NArray *cdr_narray(mempool_t *mp, NArray *src) {
    if (src->size == 0) {
        return src;
    }
    if (mp->narray_node_pool_used == NARRAY_NODE_POOL_SIZE) {
        return NULL;
    }
    NArray *node = &(mp->narray_node_pool[mp->narray_node_pool_used++]);
    node->size = src->size - 1;
    node->data = (node->size == 0) ? NULL : (src->data + 1);
    return node;
}

void end_mempool(mempool_t *mempool) {
#define MAYBE_FREE(p) { if (p != NULL) { free(p); } }

    MAYBE_FREE(mempool->lispobject_pool);
    MAYBE_FREE(mempool->string_pool);
#if !DOUBLE_IMMEDIATE
    MAYBE_FREE(mempool->float_pool);
#endif
    MAYBE_FREE(mempool->narray_node_pool);
    MAYBE_FREE(mempool);
}

mempool_t *init_mempool(void) {
    mempool_t *mempool = malloc(sizeof(mempool_t));
    if (mempool == NULL) {
        return NULL;
    }

    mempool->lispobject_pool = NULL;
    mempool->string_pool = NULL;
#if !DOUBLE_IMMEDIATE
    mempool->float_pool = NULL;
#endif
    mempool->narray_node_pool = NULL;

    Lisp_Object *lispobject_pool = malloc(sizeof(Lisp_Object) * LISPOBJECT_POOL_SIZE);
    if (lispobject_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->lispobject_pool = lispobject_pool;
    mempool->lispobject_pool_used = 0;

    char *string_pool = malloc(sizeof(char) * STRING_POOL_SIZE);
    if (string_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->string_pool = string_pool;
    mempool->string_pool_used = 0;

#if !DOUBLE_IMMEDIATE
    double *float_pool = malloc(sizeof(double) * FLOAT_POOL_SIZE);
    if (float_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->float_pool = float_pool;
    mempool->float_pool_used = 0;
#endif

    NArray *narray_node_pool = malloc(sizeof(NArray) * NARRAY_NODE_POOL_SIZE);
    if (narray_node_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->narray_node_pool = narray_node_pool;
    mempool->narray_node_pool_used = 0;

    return mempool;
}
