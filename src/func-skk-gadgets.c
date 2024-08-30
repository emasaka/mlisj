/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "lispobject.h"
#include "lispenv.h"
#include "mempool.h"
#include "eval.h"
#include "func-helper.h"
#include "util.h"

/* difinitions from skk-vars.el */
#define SKK_NUMBER_STYPE 1
#define SKK_DATE_AD LISP_NIL

#define TMP_BUFFSIZE 512

/* dummy values for variables and functions */
#define V_SKK_VERSION "MLISJ/0.0.0 (NO-CODENAME)"

/*
    Helper functions
*/

static void str_cpy_up_to_digit(char *dst, const char *src, size_t len) {
    char *d = dst;
    const char *s = src;
    size_t i = 0;
    while ( !isdigit(*s) && (*s != '\0') && (i < len)) {
        *d++ = *s++;
        i++;
    }
    *d = '\0';
}

static char *cat_2strings(lispenv_t *env, char *str1, char *str2) {
    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    char *newstr = new_string_area(env->mempool, str1_len + str2_len + 1);
    if (newstr == NULL) { return NULL; }
    char *p = newstr;
    memcpy(p, str1, str1_len);
    p += str1_len;
    memcpy(p, str2, str2_len);
    p += str2_len;
    p[0] = '\0';
    return newstr;
}

static char *cat_3strings(lispenv_t *env, char *str1, char *str2, char *str3) {
    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    size_t str3_len = strlen(str3);
    char *newstr = new_string_area(env->mempool, str1_len + str2_len + str3_len + 1);
    if (newstr == NULL) { return NULL; }
    char *p = newstr;
    memcpy(p, str1, str1_len);
    p += str1_len;
    memcpy(p, str2, str2_len);
    p += str2_len;
    memcpy(p, str3, str3_len);
    p+= str3_len;
    p[0] = '\0';
    return newstr;
}

static char *cat_4strings(lispenv_t *env, char *str1, char *str2, char *str3, char *str4) {
    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    size_t str3_len = strlen(str3);
    size_t str4_len = strlen(str4);
    char *newstr = new_string_area(env->mempool, str1_len + str2_len + str3_len + str4_len + 1);
    if (newstr == NULL) { return NULL; }
    char *p = newstr;
    memcpy(p, str1, str1_len);
    p += str1_len;
    memcpy(p, str2, str2_len);
    p += str2_len;
    memcpy(p, str3, str3_len);
    p+= str3_len;
    memcpy(p, str4, str4_len);
    p+= str4_len;
    p[0] = '\0';
    return newstr;
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
    snprintf(buff, sizeof(buff), "%d", r);
    char *str = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(str);
    return LISP_STRING(str);
}

/*
    Function: skk-gadget-units-conversion
*/

static struct {
    const char *unit_from;
    const char *unit_to;
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
                snprintf(buff,sizeof(buff), "%g", val * units_list[i].ratio);
                size_t len = strlen(buff) + strlen(units_list[i].unit_to) + 1;
                char *str = new_string_area(env->mempool, len);
                CHECK_ALLOC(str);
                snprintf(str, len, "%s%s", buff, units_list[i].unit_to);
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

int gengo_to_ad_1(const char *gengo, int year) {
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
    /* check values of month and day*/
    CHECK_CONDITION(month <= 12);
    static int days_in_a_month[] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month > 0 && day > 0) {
        CHECK_CONDITION(day <= days_in_a_month[month]);
        /* check of leap year is omitted */
    }

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
    CHECK_CONDITION(args->size >= 1 && args->size <= 4);

    bool not_gannen = (GET_TYPE(OPTIONAL_ARG(args, 3)) != Lisp_Nil);

    Lisp_Object tail_o = OPTIONAL_ARG(args, 2);
    char *tail;
    if (GET_TYPE(tail_o) == Lisp_Nil) {
        tail = "";
    } else {
        CHECK_TYPE(tail_o, Lisp_String);
        tail = GET_SVAL(tail_o);
    }

    Lisp_Object divider_o = OPTIONAL_ARG(args, 1);
    char *divider = (GET_TYPE(divider_o) == Lisp_Nil) ? "" : GET_SVAL(args->data[1]);

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
        snprintf(buff, sizeof(buff), "%d", GET_IVAL(nengo_o));
        nengo = buff;
    } else if (GET_TYPE(nengo_o) == Lisp_String) {
        nengo = GET_SVAL(nengo_o);
    } else {
        return LISP_ERROR(Evaluation_Error);
    }

    char *newstr = cat_4strings(env, gengo, divider, nengo, tail);
    CHECK_ALLOC(newstr);
    return LISP_STRING(newstr);
}


/*
    Function: skk-gengo-to-ad
*/

Lisp_Object f_skk_gengo_to_ad(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size <= 2);

    Lisp_Object tail_o = OPTIONAL_ARG(args, 1);
    char *tail;
    if (GET_TYPE(tail_o) == Lisp_Nil) {
        tail = "";
    } else {
        CHECK_TYPE(tail_o, Lisp_String);
        tail = GET_SVAL(tail_o);
    }

    Lisp_Object head_o = OPTIONAL_ARG(args, 0);
    char *head;
    if (GET_TYPE(head_o) == Lisp_Nil) {
        head = "";
    } else {
        CHECK_TYPE(head_o, Lisp_String);
        head = GET_SVAL(head_o);
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
    snprintf(buff, sizeof(buff), "%d", v);
    char *newstr = cat_3strings(env, head, buff, tail);
    CHECK_ALLOC(newstr);
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

static int month_name_to_month(const char *name) {
    for (size_t i = 0; i < (sizeof(skk_month_list) / sizeof(skk_month_list[0])); i++) {
        if (strcmp(skk_month_list[i][0], name) == 0) {
            return i + 1;
        }
    }
    return -1;
}

static char *dow_ja(const char *name) {
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
            snprintf(nengo_buff, sizeof(nengo_buff), "%d", GET_IVAL(nengo_o));
            CHECK_CONDITION(skk_num_exp(nengo_buff, num_type, buff, TMP_BUFFSIZE) == 0);
            nengo_s = buff;
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
        year_str = cat_2strings(env, gengo_s, nengo_s);
        CHECK_ALLOC(year_str);
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
    snprintf(buff, sizeof(buff), format_s, year_str, month_str, day_str, dow_str);
    char *result = copy_to_string_area(env->mempool, buff);
    CHECK_ALLOC(result);
    return LISP_STRING(result);
}

Lisp_Object f_skk_default_current_date(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 7 || args->size <= 8);

    return skk_default_current_date(
               args->data[0], args->data[1], args->data[2], args->data[3],
               args->data[4],args->data[5], args->data[6],
               OPTIONAL_ARG(args, 7),
               env );
}

/*
    Function: skk-current-date
*/

Lisp_Object split_time_string(char *str, lispenv_t *env) {
    char *saveptr;
    char *dow = strtok_r(str, " ", &saveptr);
    char *dow_s = copy_to_string_area(env->mempool, dow);
    CHECK_ALLOC(dow_s);
    char *month = strtok_r(NULL, " ", &saveptr);
    char *month_s = copy_to_string_area(env->mempool, month);
    CHECK_ALLOC(month_s);
    char *day = strtok_r(NULL, " ", &saveptr);
    char *day_s = copy_to_string_area(env->mempool, day);
    CHECK_ALLOC(day_s);
    char *time = strtok_r(NULL, " ", &saveptr);
    char *year = strtok_r(NULL, " ", &saveptr);
    char *year_s = copy_to_string_area(env->mempool, year);
    CHECK_ALLOC(year_s);

    char *hh = strtok_r(time, ":", &saveptr);
    char *hh_s = copy_to_string_area(env->mempool, hh);
    CHECK_ALLOC(hh_s);
    char *mm = strtok_r(NULL, ":", &saveptr);
    char *mm_s = copy_to_string_area(env->mempool, mm);
    CHECK_ALLOC(mm_s);
    char *ss = strtok_r(NULL, ":", &saveptr);
    char *ss_s = copy_to_string_area(env->mempool, ss);
    CHECK_ALLOC(ss_s);

    NArray *ary = new_narray(env->mempool, 7);
    CHECK_ALLOC(ary);
    ary->data[0] = LISP_STRING(year_s);
    ary->data[1] = LISP_STRING(month_s);
    ary->data[2] = LISP_STRING(day_s);
    ary->data[3] = LISP_STRING(dow_s);
    ary->data[4] = LISP_STRING(hh_s);
    ary->data[5] = LISP_STRING(mm_s);
    ary->data[6] = LISP_STRING(ss_s);
    return LISP_CLIST(ary);
}

Lisp_Object skk_current_date_1(lispenv_t *env) {
    struct tm newtime;
    char buffer[STRFTIME_BUFFSIZE];

    env->current_time_func(&newtime);
    strftime(buffer, STRFTIME_BUFFSIZE, "%a %b %d %H:%M:%S %Y", &newtime);
    return split_time_string(buffer, env);
}

static Lisp_Object skk_default_current_date_function(
        Lisp_Object date_information,
        __attribute__((unused)) Lisp_Object format,
        Lisp_Object gengo_p, Lisp_Object and_time, lispenv_t *env) {
    return skk_default_current_date(date_information, LISP_NIL, LISP_INT(SKK_NUMBER_STYPE),
            gengo_p, LISP_INT(0), LISP_INT(0), LISP_INT(0), and_time,env );
}

static Lisp_Object lisp_not(Lisp_Object x) {
    if (GET_TYPE(x) == Lisp_Nil) {
        return LISP_INT(1); /* instead of 't' */
    } else {
        return LISP_NIL;
    }
}

Lisp_Object f_skk_current_date(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size <= 3);
    Lisp_Object and_time = OPTIONAL_ARG(args, 2);
    Lisp_Object format = OPTIONAL_ARG(args, 1);
    Lisp_Object pp_function = OPTIONAL_ARG(args, 0);

    Lisp_Object datetime = skk_current_date_1(env);
    if (GET_TYPE(datetime) == Internal_Error) { return datetime; }
    if (GET_TYPE(pp_function) == Lisp_Nil) {
        return skk_default_current_date_function(
                    datetime, format, lisp_not(SKK_DATE_AD), and_time, env );
    } else {
        NArray *args = new_narray(env->mempool, 4);
        CHECK_ALLOC(args);
        args->data[0] = datetime;
        args->data[1] = format;
        args->data[2] = lisp_not(SKK_DATE_AD);
        args->data[3] = and_time;
        return call_lambda(pp_function, args, env);
    }
}

/*
    Function: skk-relative-date
*/

Lisp_Object skk_relative_date_1(int offset_yy, int offset_mm, int offset_dd, lispenv_t *env) {
    struct tm newtime;
    char buffer[STRFTIME_BUFFSIZE];

    env->current_time_func(&newtime);
    newtime.tm_year += offset_yy;
    newtime.tm_mon += offset_mm;
    newtime.tm_mday += offset_dd;
    time_t ltime = mktime(&newtime);
    localtime_r(&ltime, &newtime);

    strftime(buffer, STRFTIME_BUFFSIZE, "%a %b %d %H:%M:%S %Y", &newtime);
    return split_time_string(buffer, env);
}

Lisp_Object f_skk_relative_date(NArray *args, lispenv_t *env) {
    size_t args_size = args->size;
    CHECK_CONDITION(args_size >= 3);
    Lisp_Object pp_function = args->data[0];
    Lisp_Object format = args->data[1];
    Lisp_Object and_time = args->data[2];

    char *yy_sym = str2symbol(env->symbol_pool, ":yy", false);
    CHECK_ALLOC(yy_sym);
    char *mm_sym = str2symbol(env->symbol_pool, ":mm", false);
    CHECK_ALLOC(mm_sym);
    char *dd_sym = str2symbol(env->symbol_pool, ":dd", false);
    CHECK_ALLOC(dd_sym);

    int yy = 0; int mm = 0; int dd = 0;
    for (size_t i = 3; i < args_size; i += 2) {
        Lisp_Object elm = args->data[i];
        if (GET_TYPE(elm) == Lisp_Symbol) {
            CHECK_CONDITION(i + 1 < args_size);
            CHECK_TYPE(args->data[i + 1], Lisp_Int);
            if (GET_SVAL(elm) == yy_sym) {
                yy = GET_IVAL(args->data[i + 1]);
            } else if (GET_SVAL(elm) == mm_sym) {
                mm = GET_IVAL(args->data[i + 1]);
            } else if (GET_SVAL(elm) == dd_sym) {
                dd = GET_IVAL(args->data[i + 1]);
            }
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
    }

    Lisp_Object datetime = skk_relative_date_1(yy, mm, dd, env);
    if (GET_TYPE(datetime) == Internal_Error) { return datetime; }
    if (GET_TYPE(pp_function) == Lisp_Nil) {
        return skk_default_current_date_function(
                    datetime, format, lisp_not(SKK_DATE_AD), and_time, env );
    } else {
        NArray *args = new_narray(env->mempool, 4);
        CHECK_ALLOC(args);
        args->data[0] = datetime;
        args->data[1] = format;
        args->data[2] = SKK_DATE_AD;
        args->data[3] = and_time;
        return call_lambda(pp_function, args, env);
    }
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
    ADD_FUNC_OR_RETURN(func_pool, "skk-current-date", f_skk_current_date);
    ADD_FUNC_OR_RETURN(func_pool, "skk-relative-date", f_skk_relative_date);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "skk-num-list", DYNAMIC_VAL(dv_skk_num_list));

    return 0;
}
