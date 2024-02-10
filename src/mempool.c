#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "lispobject.h"
#include "mempool.h"

#define LISPOBJECT_POOL_SIZE 8192
#define STRING_POOL_SIZE 4096
#define FLOAT_POOL_SIZE 512
#define NARRAY_NODE_POOL_SIZE 1024

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
char *copy_to_string_area(mempool_t *mp, char *str) {
    int len = strlen(str);
    char *area = new_string_area(mp, len + 1);
    if (area == NULL) { return NULL; }
    strcpy(area, str);
    return area;
}

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

void end_mempool(mempool_t *mempool) {
#define MAYBE_FREE(p) { if (p != NULL) { free(p); } }

    MAYBE_FREE(mempool->lispobject_pool);
    MAYBE_FREE(mempool->string_pool);
    MAYBE_FREE(mempool->float_pool);
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
    mempool->float_pool = NULL;
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

    double *float_pool = malloc(sizeof(double) * FLOAT_POOL_SIZE);
    if (float_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->float_pool = float_pool;
    mempool->float_pool_used = 0;

    NArray *narray_node_pool = malloc(sizeof(NArray) * NARRAY_NODE_POOL_SIZE);
    if (narray_node_pool == NULL) {
        end_mempool(mempool);
        return NULL;
    }
    mempool->narray_node_pool = narray_node_pool;
    mempool->narray_node_pool_used = 0;

    return mempool;
}
