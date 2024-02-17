#include "lispobject.h"
#include "lispenv.h"

Lisp_Object eval_expr(Lisp_Object, lispenv_t *); /* prototype declaration */

#define EVAL_ERROR_VAL ((Lisp_Object){ .type = Internal_Error, .val.err = Evaluation_Error })

static Lisp_Object eval_func_call(Lisp_Object lst, lispenv_t *env) {
        cfunc_t func = get_func(env->func_pool, lst.val.aval->data[0].val.sval);
        if (func == NULL) {
            return EVAL_ERROR_VAL;
        }
        /* eval arguments*/
        NArray *args = new_narray(env->mempool, lst.val.aval->size - 1);
        if (args == NULL) {
            return EVAL_ERROR_VAL;
        }
        for (size_t i = 1; i < lst.val.aval->size; i++) {
            Lisp_Object ret = eval_expr(lst.val.aval->data[i], env);
            if (ret.type == Internal_Error) {
                 return ret;
            }
            args->data[i - 1] = ret;
        }
        return func(args, env);
}

static Lisp_Object eval_list(Lisp_Object lst, lispenv_t *env) {
    if (lst.val.aval->size == 0) {
        /* return () itself */
        return lst;
    }
    Lisp_Object item_first = lst.val.aval->data[0];
    if (item_first.type == Lisp_Symbol) {
        if (item_first.val.sval == env->Symbol_quote) {
            if (lst.val.aval->size == 2 ) {
                return lst.val.aval->data[1];
            } else {
                return EVAL_ERROR_VAL;
            }
        } else if (item_first.val.sval == env->Symbol_lambda) {
            /* return (lambda ...) itself*/
            return lst;
        }
        return eval_func_call(lst, env);
    }
    return EVAL_ERROR_VAL;
}

Lisp_Object eval_expr(Lisp_Object expr, lispenv_t *env) {
    switch (expr.type) {
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
        return get_variable(env->variable_pool, expr.val.sval);
        break;
    case Lisp_CList:
        return eval_list(expr, env);
        break;
    default:
        /* unkknown case */
        return EVAL_ERROR_VAL;
    }
}
