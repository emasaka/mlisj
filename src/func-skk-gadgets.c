#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    Function: skk-gadget-units-conversion
*/

static struct {
    char *unit_from;
    char *unit_to;
    double ratio;
} units_list[] = {
    { "mile" ,"km" ,1.6093 },
    { "mile" ,"yard" ,1760 },
    { "yard" ,"feet" ,3 },
    { "yard" ,"cm" ,91.44 },
    { "feet" ,"inch" ,12 },
    { "feet" ,"cm" ,30.48 },
    { "inch" ,"feet" ,0.5 },
    { "inch" ,"cm" ,2.54 }
};

Lisp_Object f_skk_gadget_units_conversion(NArray *args, lispenv_t *env) {
    char buff[TMP_BUFFSIZE];
    CHECK_CONDITION(args->size == 3);
    CHECK_TYPE(args->data[0], Lisp_String);
    char *unit_from = GET_SVAL(args->data[0]);
    /* WORKAROUND: assume original value is integer */
    CHECK_TYPE(args->data[1], Lisp_Int);
    int val = GET_IVAL(args->data[1]);
    CHECK_TYPE(args->data[2], Lisp_String);
    char *unit_to = GET_SVAL(args->data[2]);

    for (size_t i = 0; i < (sizeof(units_list) / sizeof(units_list[0])); i++) {
        if (strcmp(unit_from, units_list[i].unit_from) == 0 &&
            strcmp(unit_to, units_list[i].unit_to) == 0 ) {
                sprintf(buff, "%g", val * units_list[i].ratio);
                char *str = new_string_area(env->mempool, strlen(buff) + strlen(units_list[i].unit_to) + 1);
                CHECK_ALLOC(str);
                sprintf(str, "%s%s", buff, units_list[i].unit_to);
                return LISP_STRING(str);
        }
    }
    /* not matched */
    return LISP_ERROR(Evaluation_Error);
}

/*
    gengo related routeines
*/

static struct {
    char *gengo;
    char *gengo_kana;
    char *gengo_initial;
    int start_y;
    int start_m;
    int start_d;
} gengo_list[] = {
    { "明治", "めいじ", "M", 1868, 10, 23 },
    { "大正", "たいしょう", "T", 1912, 7, 30 },
    { "昭和", "しょうわ", "S", 1926, 12, 25 },
    { "平成", "へいせい", "H", 1989, 1, 8 },
    { "令和", "れいわ", "R", 2019, 5, 1 }
};

int gengo_to_ad_1(char *gengo, int year) {
    for (size_t i = 0; i < (sizeof(gengo_list) / sizeof(gengo_list[0])); i++) {
        if (strcmp(gengo, gengo_list[i].gengo) == 0 || strcmp(gengo, gengo_list[i].gengo_kana) == 0) {
            return year + gengo_list[i].start_y - 1;
        }
    }
    /* unknown gengo */
    return -1;
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
    ADD_FUNC_OR_RETURN(func_pool, "skk-gadget-units-conversion", f_skk_gadget_units_conversion);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
