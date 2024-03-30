#include "lispobject.h"
#include "lispenv.h"
#include "func-helper.h"

/* dummy values for variables and functions */
#define V_SKK_VERSION "MLISJ/0.0.0 (NO-CODENAME)"

/*
    Function: skk-version
*/

Lisp_Object f_skk_version(__attribute__((unused)) NArray *args, __attribute__((unused)) lispenv_t *env) {
    return LISP_STRING(V_SKK_VERSION);
}

/*
    register functions and variables
*/

int register_func_skk_gadgets(lispenv_t *env) {
    func_pool_t *func_pool = env->func_pool;
    ADD_FUNC_OR_RETURN(func_pool, "skk-version", f_skk_version);

    return 0;
}
