#include <stdio.h>
#include <stdlib.h>
#include "lispobject.h"
#include "lispenv.h"
#include "func-helper.h"

#define TMP_BUFFSIZE 512

/* dummy values for variables and functions */
#define V_SKK_VERSION "MLISJ/0.0.0 (NO-CODENAME)"

/*
    Function: skk-version
*/

Lisp_Object f_skk_version(__attribute__((unused)) NArray *args, __attribute__((unused)) lispenv_t *env) {
    return LISP_STRING(V_SKK_VERSION);
}

/*
    Function: skk-times
*/

Lisp_Object f_skk_times(NArray *args, lispenv_t *env) {
    /* WORKAROUND: assume all args are integer */
    char buff[TMP_BUFFSIZE];
    CHECK_CONDITION(args->size == 0);
    int r = 1;
    if (env->skk_num_list != NULL) {
        for (size_t i = 0; env->skk_num_list[i] != NULL; i++) {
            char *endptr;
            int n = (int)strtol(env->skk_num_list[i], &endptr, 10);
            r *= n;
        }
    }
    sprintf(buff, "%d", r);
    char *str = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(str);
    return LISP_STRING(str);
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
    ADD_FUNC_OR_RETURN(func_pool, "skk-times", f_skk_times);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
