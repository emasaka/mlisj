#include <stdbool.h>
#include "lispobject.h"
#include "lispenv.h"

/*
    Function: -
*/

static Lisp_Object minus_onearg(NArray *args, lispenv_t *env) {
    /* (- x) -> -x */
    if (GET_TYPE(args->data[0]) == Lisp_Int) {
        return LISP_INT(-GET_IVAL(args->data[0]));
    } else if (GET_TYPE(args->data[0]) == Lisp_Float) {
        double *flt = cdouble2float(env->mempool, -(*GET_FVAL(args->data[0])));
        if (flt == NULL) {
            return LISP_ERROR(Memory_Error);
        } else {
            return LISP_FLOAT(flt);
        }
    } else {
        return LISP_ERROR(Evaluation_Error);
    }
}

static Lisp_Object minus_multiarg(NArray *args, lispenv_t *env) {
    int r_int = 0;
    double r_double = 0.0;
    bool float_p = false;

    if (GET_TYPE(args->data[0]) == Lisp_Int) {
        r_int = GET_IVAL(args->data[0]);
    } else if (GET_TYPE(args->data[0]) == Lisp_Float) {
        r_double = *GET_FVAL(args->data[0]);
        float_p = true;
    } else {
        return LISP_ERROR(Evaluation_Error);
    }

    for (size_t i = 1; i < args->size; i++) {
        if (GET_TYPE(args->data[i]) == Lisp_Int) {
            if (float_p == true) {
                r_double -= (double)GET_IVAL(args->data[i]);
            } else {
                r_int -= GET_IVAL(args->data[i]);
            }
        } else if (GET_TYPE(args->data[i]) == Lisp_Float) {
            if (float_p == true) {
                r_double -= *GET_FVAL(args->data[i]);
            } else {
                r_double = (double)r_int;
                r_double -= *GET_FVAL(args->data[i]);
                float_p = true;
            }
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
    }

    if (float_p == true) {
        double *flt = cdouble2float(env->mempool, r_double);
        if (flt == NULL) {
            return LISP_ERROR(Memory_Error);
        } else {
            return LISP_FLOAT(flt);
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
    Function: car
*/

Lisp_Object f_car(NArray *args, __attribute__((unused)) lispenv_t *env) {
    if (args->size == 1) {
        if (GET_TYPE(args->data[0]) == Lisp_CList) {
            if (GET_AVAL(args->data[0])->size == 0) {
                return LISP_NIL;
            } else {
                return GET_AVAL(args->data[0])->data[0];
            }
        } else if (GET_TYPE(args->data[0]) == Lisp_Nil) {
            return LISP_NIL;
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
    } else {
        return LISP_ERROR(Evaluation_Error);
    }
}

/*
    register functions
*/

int register_func_simple(func_pool_t *func_pool) {
    int r;

    r = add_func_from_cstr(func_pool, "-", f_minus, false);
    if (r != 0) { return -1; }
    r = add_func_from_cstr(func_pool, "car", f_car, false);
    if (r != 0) { return -1; }

    return 0;
}
