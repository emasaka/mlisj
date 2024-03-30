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
    Dynamic variable: skk-num-list
*/

Lisp_Object dv_skk_num_list(lispenv_t *env) {
    if (env->skk_num_list == NULL) { return LISP_NIL; }

    size_t sz;
    for (sz = 0; env->skk_num_list[sz] != NULL; sz++) ;

    NArray *ary = new_narray(env->mempool, sz);
    CHECK_ALLOC(ary);
    for (size_t i = 0; i < sz; i++) {
        char *str = copy_to_string_area(env->mempool, env->skk_num_list[i]);
        CHECK_ALLOC(str);
        ary->data[i] = LISP_STRING(str);
    }
    return LISP_CLIST(ary);
}

/*
    register functions and variables
*/

int register_func_skk_gadgets(lispenv_t *env) {
    func_pool_t *func_pool = env->func_pool;
    ADD_FUNC_OR_RETURN(func_pool, "skk-version", f_skk_version);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
