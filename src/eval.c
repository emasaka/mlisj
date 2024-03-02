#include "lispobject.h"
#include "lispenv.h"

Lisp_Object eval_expr(Lisp_Object, lispenv_t *); /* prototype declaration */

static Lisp_Object eval_func_call(Lisp_Object lst, lispenv_t *env) {
    cfunc_t func = get_func(env->func_pool, GET_SVAL(GET_AVAL(lst)->data[0]));
    if (func == NULL) {
        return LISP_ERROR(Evaluation_Error);
    }
    /* eval arguments*/
    NArray *args = new_narray(env->mempool, GET_AVAL(lst)->size - 1);
    if (args == NULL) {
        return LISP_ERROR(Evaluation_Error);
    }
    size_t sz = GET_AVAL(lst)->size;
    for (size_t i = 1; i < sz; i++) {
        Lisp_Object ret = eval_expr(GET_AVAL(lst)->data[i], env);
        if (GET_TYPE(ret) == Internal_Error) {
            return ret;
        }
        args->data[i - 1] = ret;
    }
    return func(args, env);
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
        return eval_func_call(lst, env);
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
        /* symbol as variable */
        return get_variable(env->variable_pool, GET_SVAL(expr));
        break;
    case Lisp_CList:
        return eval_list(expr, env);
        break;
    default:
        /* unkknown case */
        return LISP_ERROR(Evaluation_Error);
    }
}
