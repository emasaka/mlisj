/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <string.h>
#include "mlisj.h"
#include "lispobject.h"
#include "lispenv.h"
#include "reader.h"
#include "eval.h"
#include "func-simple.h"
#include "func-skk-gadgets.h"

static int mlisj_init(char **skk_num_list, const char *skk_henkan_key, lispenv_t * env) {
    if (register_func_simple(env) != 0) {
        end_lispenv(env);
        return -1;
    }
    if (register_func_skk_gadgets(env) != 0) {
        end_lispenv(env);
        return -1;
    }
    register_skk_num_list(env, skk_num_list);
    register_skk_henkan_key(env, skk_henkan_key);

    return 0;
}

int mlisj_eval(const char *src, char *dest, size_t size, char **skk_num_list, const char *skk_henkan_key) {
    /* initialize*/
    lispenv_t *env = init_lispenv();
    if (env == NULL) { return MLISJ_ERROR_RUNTIME; }

    int exitcode = 0;
    if (mlisj_init(skk_num_list, skk_henkan_key, env) != 0) {
        exitcode = MLISJ_ERROR_RUNTIME;
        goto exit;
    }

    /* read */
    Lisp_Object lisp_expr = reader(src, env);
    if (GET_TYPE(lisp_expr) == Internal_Error) {
        exitcode = (GET_ERROR_TYPE(lisp_expr) == Reader_Error) ?
                MLISJ_ERROR_EVALUATION :
                MLISJ_ERROR_RUNTIME;
        goto exit;
    }

    /* evaluate */
    Lisp_Object result = eval_expr(lisp_expr, env);
    switch (GET_TYPE(result)) {
    case Lisp_String:
        strncpy(dest, GET_SVAL(result), size);
        if (size > 0) { dest[size - 1] = 0; }
        break;
    case Internal_Error:
        exitcode = (GET_ERROR_TYPE(result) == Evaluation_Error) ?
                MLISJ_ERROR_EVALUATION : MLISJ_ERROR_RUNTIME;
        break;
    default:
        exitcode = MLISJ_ERROR_EVALUATION;
        break;
    }

exit:
    end_lispenv(env);
    return exitcode;
}
