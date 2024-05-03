#include "lispobject.h"
#include "lispenv.h"

Lisp_Object eval_expr(Lisp_Object, lispenv_t *); /* prototype declaration */

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
        Lisp_Object (*fn)() =GET_FNVAL(val);
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
