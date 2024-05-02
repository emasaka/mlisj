#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lispobject.h"
#include "lispenv.h"
#include "mempool.h"
#include "func-helper.h"

#define TMP_BUFFSIZE 512

/* dummy values for variables and functions */
#define V_SKK_VERSION "MLISJ/0.0.0 (NO-CODENAME)"

/*
    Helper functions
*/

static void str_cpy_up_to_digit(char *dst, char *src, size_t len) {
    char *d = dst;
    char *s = src;
    size_t i = 0;
    while ( !isdigit(*s) && (*s != '\0') && (i < len)) {
        *d++ = *s++;
        i++;
    }
    *d = '\0';
}

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
    char buff[INT_STRLEN + 1];
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

/* month and day can be 0 (unspecified)*/
Lisp_Object ad_to_gengo_1(lispenv_t *env, int ad, bool not_gannen, int month, int day) {
    size_t i = 0;
    for ( ; i < sizeof(gengo_list) / sizeof(gengo_list[0]); i++) {
        if ((ad * 10000 + (month ? month : 13) * 100 + (day ? day : 32)) <
            (gengo_list[i].start_y * 10000 + gengo_list[i].start_m * 100 + gengo_list[i].start_d) ) {
            break;
        }
    }
    if (i == 0) {
        /* before first gengo */
        return LISP_ERROR(Evaluation_Error);
    }
    int year = ad - gengo_list[i - 1].start_y + 1;
    NArray *ary = new_narray(env->mempool, 3);
    CHECK_ALLOC(ary);
    ary->data[0] = LISP_STRING(gengo_list[i - 1].gengo);
    ary->data[1] = LISP_STRING(gengo_list[i - 1].gengo_initial);
    ary->data[2] = (!not_gannen && year == 1) ? LISP_STRING("元") : LISP_INT(year);
    return LISP_CLIST(ary);
}

/*
    Function: f_skk_ad_to_gengo
*/

Lisp_Object f_skk_ad_to_gengo(NArray *args, lispenv_t *env) {
    char *divider;
    char *tail;
    bool not_gannen;

    CHECK_CONDITION(args->size >= 1 && args->size <= 4);

    if (args->size < 4 || GET_TYPE(args->data[3]) == Lisp_Nil) {
        not_gannen = false;
    } else {
        not_gannen = true;
    }

    if (args->size < 3 || GET_TYPE(args->data[2]) == Lisp_Nil) {
        tail = "";
    } else {
        CHECK_TYPE(args->data[2], Lisp_String);
        tail = GET_SVAL(args->data[2]);
    }

    if (args->size < 2 || GET_TYPE(args->data[1]) == Lisp_Nil) {
        divider = "";
    } else {
        CHECK_TYPE(args->data[1], Lisp_String);
        divider = GET_SVAL(args->data[1]);
    }

    CHECK_TYPE(args->data[0], Lisp_Int);
    size_t gengo_index = GET_IVAL(args->data[0]);
    CHECK_CONDITION(gengo_index == 0 || gengo_index == 1);

    CHECK_CONDITION(env->skk_num_list != NULL);
    char *endptr;
    int ad = (int)strtol(env->skk_num_list[0], &endptr, 10);

    Lisp_Object v = ad_to_gengo_1(env, ad, not_gannen, 0, 0);
    if (GET_TYPE(v) == Internal_Error) { return v; }

    char *gengo = GET_SVAL(GET_AVAL(v)->data[gengo_index]);
    Lisp_Object nengo_o = GET_AVAL(v)->data[2];
    char *nengo;
    char buff[INT_STRLEN + 1];
    if (GET_TYPE(nengo_o) == Lisp_Int) {
        sprintf(buff, "%d", GET_IVAL(nengo_o));
        nengo = buff;
    } else if (GET_TYPE(nengo_o) == Lisp_String) {
        nengo = GET_SVAL(nengo_o);
    } else {
        return LISP_ERROR(Evaluation_Error);
    }

    size_t len = strlen(gengo) + strlen(divider) + strlen(nengo) + strlen(tail);
    char *newstr = new_string_area(env->mempool, len + 1);
    CHECK_ALLOC(newstr);
    strncpy(newstr, gengo, len);
    strncat(newstr, divider, len);
    strncat(newstr, nengo, len);
    strncat(newstr, tail, len);
    return LISP_STRING(newstr);
}


/*
    Function: f_skk_gengo_to_ad
*/

Lisp_Object f_skk_gengo_to_ad(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size <= 2);

    char *tail;
    if (args->size < 2 || GET_TYPE(args->data[1]) == Lisp_Nil) {
        tail = "";
    } else {
        CHECK_TYPE(args->data[1], Lisp_String);
        tail = GET_SVAL(args->data[1]);
    }

    char *head;
    if (args->size < 1 || GET_TYPE(args->data[0]) == Lisp_Nil) {
        head = "";
    } else {
        CHECK_TYPE(args->data[0], Lisp_String);
        head = GET_SVAL(args->data[0]);
    }
    CHECK_CONDITION(env->skk_num_list != NULL);
    char *endptr;
    int nengo = (int)strtol(env->skk_num_list[0], &endptr, 10);

    CHECK_CONDITION(env->skk_henkan_key != NULL);
    char gengo_buf[TMP_BUFFSIZE];
    str_cpy_up_to_digit(gengo_buf, env->skk_henkan_key, TMP_BUFFSIZE);

    int v = gengo_to_ad_1(gengo_buf, nengo);
    CHECK_CONDITION(v != -1);

    char buff[INT_STRLEN + 1];
    sprintf(buff, "%d", v);
    size_t len = strlen(head) + strlen(buff) + strlen(tail);
    char *newstr = new_string_area(env->mempool, len + 1);
    CHECK_ALLOC(newstr);
    strncpy(newstr, head, len);
    strncat(newstr, buff, len);
    strncat(newstr, tail, len);
    return LISP_STRING(newstr);
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
    ADD_FUNC_OR_RETURN(func_pool, "skk-ad-to-gengo", f_skk_ad_to_gengo);
    ADD_FUNC_OR_RETURN(func_pool, "skk-gengo-to-ad", f_skk_gengo_to_ad);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
