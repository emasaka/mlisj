#include <stdbool.h>
#include "lispobject.h"
#include "lispenv.h"

#define EVAL_ERROR_VAL ((Lisp_Object){ .type = Internal_Error, .val.err = Evaluation_Error })
#define MEMORY_ERROR_VAL ((Lisp_Object){ .type = Internal_Error, .val.err = Memory_Error })

/*
    Function: -
*/

static Lisp_Object minus_onearg(NArray *args, lispenv_t *env) {
    /* (- x) -> -x */
    if (args->data[0].type == Lisp_Int) {
        return LISP_INT(-(args->data[0].val.ival));
    } else if (args->data[0].type == Lisp_Float) {
        double *flt = cdouble2float(env->mempool, -(*(args->data[0].val.fval)));
        if (flt == NULL) {
            return MEMORY_ERROR_VAL;
        } else {
            return (Lisp_Object){ .type = Lisp_Float, .val.fval = flt };
        }
    } else {
        return EVAL_ERROR_VAL;
    }
}

static Lisp_Object minus_multiarg(NArray *args, lispenv_t *env) {
    int r_int = 0;
    double r_double = 0.0;
    bool float_p = false;

    if (args->data[0].type == Lisp_Int) {
        r_int = args->data[0].val.ival;
    } else if (args->data[0].type == Lisp_Float) {
        r_double = *(args->data[0].val.fval);
        float_p = true;
    } else {
        return EVAL_ERROR_VAL;
    }

    for (size_t i = 1; i < args->size; i++) {
        if (args->data[i].type == Lisp_Int) {
            if (float_p == true) {
                r_double -= (double)(args->data[i].val.ival);
            } else {
                r_int -= args->data[i].val.ival;
            }
        } else if (args->data[i].type == Lisp_Float) {
            if (float_p == true) {
                r_double -= *(args->data[i].val.fval);
            } else {
                r_double = (double)r_int;
                r_double -= *(args->data[i].val.fval);
                float_p = true;
            }
        } else {
            return EVAL_ERROR_VAL;
        }
    }

    if (float_p == true) {
        double *flt = cdouble2float(env->mempool, r_double);
        if (flt == NULL) {
            return MEMORY_ERROR_VAL;
        } else {
            return (Lisp_Object){ .type = Lisp_Float, .val.fval = flt };
        }
    } else {
        return LISP_INT(r_int);
    }

}

Lisp_Object f_minus(NArray *args, lispenv_t *env) {
    if (args->size == 0) {
        /* (-) -> 0 */
        return LISP_INT(0);
    } else if (args->size == 1) {
        return minus_onearg(args, env);
    } else {
        return minus_multiarg(args, env);
    }
}

/*
    register functions
*/

int register_func_simple(func_pool_t *func_pool) {
    int r;
    r = add_func_from_cstr(func_pool, "-", f_minus, false);
    if (r != 0) { return -1; };
    return 0;
}
