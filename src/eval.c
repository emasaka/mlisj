/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include "lispobject.h"
#include "lispenv.h"
#include "func-helper.h"

Lisp_Object eval_expr(Lisp_Object, lispenv_t *); /* prototype declaration */

Lisp_Object call_lambda(Lisp_Object lambda_lst, NArray *args, lispenv_t *env) {
    CHECK_TYPE(lambda_lst, Lisp_CList);
    size_t lambda_size = GET_AVAL(lambda_lst)->size;
    CHECK_CONDITION(lambda_size >= 2);
    CHECK_TYPE(GET_AVAL(lambda_lst)->data[0], Lisp_Symbol);
    CHECK_CONDITION(GET_SVAL(GET_AVAL(lambda_lst)->data[0]) == env->Symbol_lambda);

    Lisp_Object params = GET_AVAL(lambda_lst)->data[1];
    CHECK_TYPE(params, Lisp_CList);
    CHECK_CONDITION(GET_AVAL(params)->size == args->size);
    int var_stat = save_variable_status(env->variable_pool);
    for (size_t i = 0; i < args->size; i++) {
        Lisp_Object param = GET_AVAL(params)->data[i];
        CHECK_TYPE(param, Lisp_Symbol);
        if (set_variable(env->variable_pool, GET_SVAL(param), args->data[i]) != 0) {
            return LISP_ERROR(Evaluation_Error);
        }
    }

    Lisp_Object r = LISP_NIL;
    for (size_t i = 2; i < lambda_size; i++) {
        r = eval_expr(GET_AVAL(lambda_lst)->data[i], env);
        if (GET_TYPE(r) == Internal_Error) {
            break;
        }
    }
    restore_variable_status(env->variable_pool, var_stat);
    return r;
}

static Lisp_Object eval_symbol(Lisp_Object sym, lispenv_t *env) {
    char *s = GET_SVAL(sym);
    if (s[0] == ':') {
        /* keyword symbol*/
        return sym;
    }
    /* symbol as variable */
    Lisp_Object val = get_variable(env->variable_pool, s);
    if (GET_TYPE(val) == Dynamic_Val) {
        /* dynamic variable */
        Lisp_Object (*fn)(lispenv_t *) = GET_FNVAL(val);
        val = fn(env);
    }
    return val;
}

static Lisp_Object eval_args(NArray *args, lispenv_t *env) {
    NArray *new_args = new_narray(env->mempool, args->size);
    if (args == NULL) {
        return LISP_ERROR(Evaluation_Error);
    }
    size_t sz = args->size;
    for (size_t i = 0; i < sz; i++) {
        Lisp_Object ret = eval_expr(args->data[i], env);
        if (GET_TYPE(ret) == Internal_Error) {
            return ret;
        }
        new_args->data[i] = ret;
    }
    return LISP_CLIST(new_args);
}

static Lisp_Object eval_list(Lisp_Object lst, lispenv_t *env) {
    if (GET_AVAL(lst)->size == 0) {
        /* return () itself */
        return lst;
    }
    Lisp_Object item_first = GET_AVAL(lst)->data[0];
    if (GET_TYPE(item_first) == Lisp_Symbol) {
        if (GET_SVAL(item_first) == env->Symbol_quote) {
            if (GET_AVAL(lst)->size == 2 ) {
                return GET_AVAL(lst)->data[1];
            } else {
                return LISP_ERROR(Evaluation_Error);
            }
        } else if (GET_SVAL(item_first) == env->Symbol_lambda) {
            /* return (lambda ...) itself*/
            return lst;
        }
        /* eval function call */
        cfunc_t func = get_func(env->func_pool, GET_SVAL(GET_AVAL(lst)->data[0]));
        if (func == NULL) {
            return LISP_ERROR(Evaluation_Error);
        }
        Lisp_Object new_args = eval_args(cdr_narray(env->mempool, GET_AVAL(lst)), env);
        if (GET_TYPE(new_args) == Internal_Error) {
            return new_args;
        }
        return func(GET_AVAL(new_args), env);
    }
    return LISP_ERROR(Evaluation_Error);
}

Lisp_Object eval_expr(Lisp_Object expr, lispenv_t *env) {
    switch (GET_TYPE(expr)) {
    case Lisp_Int:
        /* fall through */
    case Lisp_Float:
        /* fall through */
    case Lisp_String:
        /* fall through */
    case Lisp_Nil:
        return expr;
        break;
    case Lisp_Symbol:
        return eval_symbol(expr, env);
        break;
    case Lisp_CList:
        return eval_list(expr, env);
        break;
    default:
        /* unkknown case */
        return LISP_ERROR(Evaluation_Error);
    }
}
