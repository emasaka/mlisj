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

static Lisp_Object lispobject_pool[LISPOBJECT_POOL_SIZE];

static int lispobject_pool_used = 0;

/* obtain new lispobject pool area */
Lisp_Object *new_lispobject_array(size_t n) {
    if (lispobject_pool_used + n > LISPOBJECT_POOL_SIZE) {
        return NULL;
    } else {
        Lisp_Object *p =  &lispobject_pool[lispobject_pool_used];
        lispobject_pool_used += n;
        return p;
    }
}

/*
  String Pool
 */

static char string_pool[STRING_POOL_SIZE];

static int string_pool_used = 0;

/* obtain new string pool area */
char *new_string_area(size_t n) {
    if (string_pool_used + n > STRING_POOL_SIZE) {
        return NULL;
    } else {
        char *p =  &string_pool[string_pool_used];
        string_pool_used += n;
        return p;
    }
}

/* copy C string to string pool */
char *copy_to_string_area(char *str) {
    int len = strlen(str);
    char *area = new_string_area(len + 1);
    if (area == NULL) { return NULL; }
    strcpy(area, str);
    return area;
}

/*
  Float Pool
 */

static double float_pool[FLOAT_POOL_SIZE];

static int float_pool_used = 0;

/* obtain new float pool area and copy number */
double *cdouble2float(double f) {
    if (float_pool_used == FLOAT_POOL_SIZE) {
        return NULL;
    } else {
        double *p = &float_pool[float_pool_used++];
        *p = f;
        return p;
    }
}

/*
  Array Node Pool
  (bodies are on lispobject_pool)
 */

static NArray narray_node_pool[NARRAY_NODE_POOL_SIZE];

static int narray_node_pool_used = 0;

/* obtain new narray  */
NArray *new_narray(size_t n) {
    if (narray_node_pool_used == NARRAY_NODE_POOL_SIZE) {
        return NULL;
    }
    Lisp_Object *array;
    if (n > 0) {
        array = new_lispobject_array(n);
        if (array == NULL) { return NULL; }
    } else {
        array = NULL;
    }
    NArray *node = &narray_node_pool[narray_node_pool_used++];
    node->size = n;
    node->data = array;
    return node;
}
