#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "lispobject.h"
#include "lispenv.h"
#include "util.h"

#define CHECK_CONDITION(c) if (!(c)) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_TYPE(x, tp) if (GET_TYPE(x) != tp) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_ALLOC(p) if ((p) == NULL) { return LISP_ERROR(Memory_Error); }

#define ADD_FUNC_OR_RETURN(fp, s, f) if(add_func_from_cstr(fp, s, f, false) != 0) { return -1; }

/*
    Function: -
*/

static Lisp_Object minus_onearg(NArray *args, lispenv_t *env) {
    /* (- x) -> -x */
    if (GET_TYPE(args->data[0]) == Lisp_Int) {
        return LISP_INT(-GET_IVAL(args->data[0]));
    } else if (GET_TYPE(args->data[0]) == Lisp_Float) {
        double *flt = cdouble2float(env->mempool, -(*GET_FVAL(args->data[0])));
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
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
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
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
    CHECK_CONDITION(args->size == 1);
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
}

/*
    Function: concat
*/

Lisp_Object f_concat(NArray *args, lispenv_t *env) {
    if (args->size == 0) {
        /* returns empty string */
        char *str = copy_to_string_area(env->mempool, "");
        CHECK_ALLOC(str);
        return LISP_STRING(str);
    } else if (args->size == 1) {
        /* returns first argument itself */
        CHECK_TYPE(args->data[0], Lisp_String);
        return args->data[0];
    } else {
        /* concatenate strings */
        size_t len = 0;
        for (size_t i = 0; i < args->size; i++) {
            CHECK_TYPE(args->data[i], Lisp_String);
            len += strlen(GET_SVAL(args->data[i]));
        }
        char *newstr = new_string_area(env->mempool, len + 1);
        CHECK_ALLOC(newstr);
        char *p = newstr;
        for (size_t i = 0; i < args->size; i++) {
            char *src = GET_SVAL(args->data[i]);
            while (*src != '\0') {
                *p++ = *src++;
            }
        }
        *p = '\0';
        return LISP_STRING(newstr);
    }
}

/*
    Function: make-string
*/

Lisp_Object f_make_string(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 2);
    CHECK_TYPE(args->data[0], Lisp_Int);
    CHECK_TYPE(args->data[1], Lisp_Int);
    int n = GET_IVAL(args->data[0]);
    CHECK_CONDITION(n >= 0);
    char c = GET_IVAL(args->data[1]);
    char *str = new_string_area(env->mempool, n + 1);
    CHECK_ALLOC(str);
    for (size_t i = 0; i < (size_t)n; i++) {
        str[i] = c;
    }
    str[n] = '\0';
    return LISP_STRING(str);
}

/*
    Function: string-to-char
*/

Lisp_Object f_string_to_char(NArray *args, __attribute__((unused)) lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_String);
    char *str = GET_SVAL(args->data[0]);
    return LISP_INT(str[0]);
}

/*
    Function: symbol-value
*/

Lisp_Object f_symbol_value(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_Symbol);
    return get_variable(env->variable_pool, GET_SVAL(args->data[0]));
}

/*
    Function: substring
*/

Lisp_Object f_substring(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 3);
    CHECK_TYPE(args->data[0], Lisp_String);
    CHECK_TYPE(args->data[1], Lisp_Int);
    CHECK_TYPE(args->data[2], Lisp_Int);

    char *str = GET_SVAL(args->data[0]);
    size_t slen = strlen(str);
    int from = GET_IVAL(args->data[1]);
    int to = GET_IVAL(args->data[2]);
    CHECK_CONDITION(0 <= from && from <= (int)slen);
    CHECK_CONDITION(from <= to && to <= (int)slen);

    size_t newlen = to - from;
    char *newstr = new_string_area(env->mempool, newlen + 1);
    CHECK_ALLOC(newstr);
    for (size_t i = 0; i < newlen; i++) {
        newstr[i] = str[from + i];
    }
    newstr[newlen] = '\0';
    return LISP_STRING(newstr);
}

/*
    Function: string-to-number
*/

Lisp_Object f_string_to_number(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_String);
    char *str = GET_SVAL(args->data[0]);
    check_num_t result = check_num_str(str, false);
    if (result == R_INT) {
        /* integer*/
        char *endptr;
        int n = (int)strtol(str, &endptr, 10);
        return LISP_INT(n);
    } else if (result == R_FLOAT) {
        /* float */
        char *endptr;
        double fnum = strtod(str, &endptr);
        double *flt = cdouble2float(env->mempool, fnum);
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
    } else {
        /* not number*/
        return LISP_INT(0);
    }
}

/*
    register functions
*/

int register_func_simple(func_pool_t *func_pool) {
    ADD_FUNC_OR_RETURN(func_pool, "-", f_minus);
    ADD_FUNC_OR_RETURN(func_pool, "car", f_car);
    ADD_FUNC_OR_RETURN(func_pool, "concat", f_concat);
    ADD_FUNC_OR_RETURN(func_pool, "make-string", f_make_string);
    ADD_FUNC_OR_RETURN(func_pool, "string-to-char", f_string_to_char);
    ADD_FUNC_OR_RETURN(func_pool, "symbol-value", f_symbol_value);
    ADD_FUNC_OR_RETURN(func_pool, "substring", f_substring);
    ADD_FUNC_OR_RETURN(func_pool, "string-to-number", f_string_to_number);

    return 0;
}
