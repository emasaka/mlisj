#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lispobject.h"
#include "lispenv.h"
#include "mempool.h"
#include "func-helper.h"
#include "util.h"

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
    Function: skk-ad-to-gengo
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
    strcpy(newstr, gengo);
    strcat(newstr, divider);
    strcat(newstr, nengo);
    strcat(newstr, tail);
    return LISP_STRING(newstr);
}


/*
    Function: skk-gengo-to-ad
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
    strcpy(newstr, head);
    strcat(newstr, buff);
    strcat(newstr, tail);
    return LISP_STRING(newstr);
}

/*
    Function: skk-default-current-date
*/

typedef char *str_tuple3[3];

static str_tuple3 skk_month_list[] = {
    {"Jan", "1", "Januar"}, {"Feb", "2", "Februar"}, {"Mar", "3", "März"},
    {"Apr", "4", "April"}, {"May", "5", "Mai"},
    {"Jun", "6", "Juni"}, {"Jul", "7", "Juli"}, {"Aug", "8", "August"},
    {"Sep", "9", "September"}, {"Oct", "10", "Oktober"},
    {"Nov", "11", "November"}, {"Dec", "12", "Dezember"}
};

static str_tuple3 skk_day_of_week_list[] = {
    {"Sun", "日", "So"}, {"Mon", "月", "Mo"}, {"Tue", "火", "Di"}, {"Wed", "水", "Mi"},
    {"Thu", "木", "Do"}, {"Fri", "金", "Fr"}, {"Sat", "土", "Sa"}
};

static int month_name_to_month(char *name) {
    for (size_t i = 0; i < (sizeof(skk_month_list) / sizeof(skk_month_list[0])); i++) {
        if (strcmp(skk_month_list[i][0], name) == 0) {
            return i + 1;
        }
    }
    return -1;
}

static char *dow_ja(char *name) {
    for (size_t i = 0; i < (sizeof(skk_day_of_week_list) / sizeof(skk_day_of_week_list[0])); i++) {
        if (strcmp(skk_day_of_week_list[i][0], name) == 0) {
            return skk_day_of_week_list[i][1];
        }
    }
    return NULL;
}

Lisp_Object skk_default_current_date(
        Lisp_Object date_information,
        Lisp_Object format,
        Lisp_Object num_type_o,
        Lisp_Object gengo_p,
        Lisp_Object gengo_index_o, /* assumed not nil */
        __attribute__((unused)) Lisp_Object month_alist_index,
        Lisp_Object dayofweek_alist_index, /* assumed 0 or nil */
        __attribute__((unused)) Lisp_Object and_time,
        lispenv_t *env ) {

    char buff[TMP_BUFFSIZE];

    CHECK_TYPE(date_information, Lisp_CList);
    CHECK_CONDITION(GET_AVAL(date_information)->size == 7);
    Lisp_Object *date_inf = GET_AVAL(date_information)->data;
    CHECK_TYPE(date_inf[0], Lisp_String);
    char *year = GET_SVAL(date_inf[0]);
    CHECK_TYPE(date_inf[1], Lisp_String);
    char *month = GET_SVAL(date_inf[1]);
    CHECK_TYPE(date_inf[2], Lisp_String);
    char *day = GET_SVAL(date_inf[2]);
    CHECK_TYPE(date_inf[3], Lisp_String);
    char *day_of_week = GET_SVAL(date_inf[3]);
    /* hour, minute, minute are ignored */
    /* Lisp_Object hour = date_information[4]; */
    /* Lisp_Object minute = date_information[5]; */
    /* Lisp_Object second = date_information[6]; */

    CHECK_TYPE(num_type_o, Lisp_Int);
    int num_type = GET_IVAL(num_type_o);
    CHECK_TYPE(gengo_index_o, Lisp_Int);
    int gengo_index = GET_IVAL(gengo_index_o);

    char *endptr;
    int year_i = (int)strtol(year, &endptr, 10);
    int month_i = month_name_to_month(month);
    int day_i = (int)strtol(day, &endptr, 10);

    char *year_str;
    if (GET_TYPE(gengo_p) == Lisp_Nil) {
        CHECK_CONDITION(skk_num_exp(year, num_type, buff, TMP_BUFFSIZE) == 0);
        year_str = copy_to_string_area(env->mempool, buff);
        CHECK_ALLOC(year_str);
    } else {
        Lisp_Object v = ad_to_gengo_1(env, year_i, false, month_i, day_i);
        CHECK_TYPE(v, Lisp_CList);
        Lisp_Object gengo_o = GET_AVAL(v)->data[gengo_index];
        CHECK_TYPE(gengo_o, Lisp_String);
        char *gengo_s = GET_SVAL(gengo_o);
        char *nengo_s;
        Lisp_Object nengo_o = GET_AVAL(v)->data[2];
        if (GET_TYPE(nengo_o) == Lisp_String) {
            nengo_s = GET_SVAL(nengo_o);
        } else if (GET_TYPE(nengo_o) == Lisp_Int) {
            char nengo_buff[INT_STRLEN + 1];
            sprintf(nengo_buff, "%d", GET_IVAL(nengo_o));
            CHECK_CONDITION(skk_num_exp(nengo_buff, num_type, buff, TMP_BUFFSIZE) == 0);
            nengo_s = buff;
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
        year_str = new_string_area(env->mempool, strlen(gengo_s) + strlen(nengo_s) + 1);
        CHECK_ALLOC(year_str);
        strcpy(year_str, gengo_s);
        strcat(year_str, nengo_s);
    }

    CHECK_CONDITION(skk_num_exp(skk_month_list[month_i - 1][1], num_type, buff, TMP_BUFFSIZE) == 0);
    char *month_str = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(month_str);

    CHECK_CONDITION(skk_num_exp(day, num_type, buff, TMP_BUFFSIZE) == 0);
    char *day_str = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(day_str);

    char *dow_str = (GET_TYPE(dayofweek_alist_index) == Lisp_Nil) ? day_of_week : dow_ja(day_of_week);

    char *format_s;
    if (GET_TYPE(format) == Lisp_Nil) {
        format_s = "%s年%s月%s日(%s)";
    } else {
        CHECK_TYPE(format, Lisp_String);
        format_s = GET_SVAL(format);
    }
    sprintf(buff, format_s, year_str, month_str, day_str, dow_str);
    char *result = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(result);
    return LISP_STRING(result);
}

Lisp_Object f_skk_default_current_date(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 7 || args->size <= 8);

    return skk_default_current_date(
               args->data[0], args->data[1], args->data[2], args->data[3],
               args->data[4],args->data[5], args->data[6],
               ((args->size == 7) ? LISP_NIL : args->data[7]),
               env );
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
    ADD_FUNC_OR_RETURN(func_pool, "skk-default-current-date", f_skk_default_current_date);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
