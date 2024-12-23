/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include "lispobject.h"
#include "lispenv.h"
#include "util.h"
#include "func-helper.h"

#define EXPOSE_SYSEM_ENV 1

#define GETPWUID_R_BUFFSIZE 1024

/* dummy values for variables and functions */
#define V_FILL_COLUMN 70
#define V_COMMENT_START "#"
#define V_CWD "/"
#define V_USER_FULL_NAME "Jay Doe"
#define DUMMY_USER_NAME "jaydoe"
#define V_USER_MAIL_ADDRESS "jaydoe@example.com"
#define V_WINDOW_WIDTH 80

/* workaround for non-Linux system*/
#if !defined(HOST_NAME_MAX)
#define HOST_NAME_MAX 64
#endif
#if !defined(LOGIN_NAME_MAX)
#define LOGIN_NAME_MAX 256
#endif

/*
    MARK: Function: -
*/

#if DOUBLE_IMMEDIATE
static Lisp_Object minus_onearg(NArray *args, __attribute__((unused)) lispenv_t *env) {
#else
static Lisp_Object minus_onearg(NArray *args, lispenv_t *env) {
#endif
    /* (- x) -> -x */
    if (GET_TYPE(args->data[0]) == Lisp_Int) {
        return LISP_INT(-GET_IVAL(args->data[0]));
    } else if (GET_TYPE(args->data[0]) == Lisp_Float) {
#if DOUBLE_IMMEDIATE
        return LISP_FLOAT(-(GET_FVAL_VAL(args->data[0])));
#else
        double *flt = cdouble2float(env->mempool, -(GET_FVAL_VAL(args->data[0])));
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
#endif
    } else {
        return LISP_ERROR(Evaluation_Error);
    }
}

#if DOUBLE_IMMEDIATE
static Lisp_Object minus_multiarg(NArray *args) {
#else
static Lisp_Object minus_multiarg(NArray *args, lispenv_t *env) {
#endif
    int r_int = 0;
    double r_double = 0.0;
    bool float_p = false;

    if (GET_TYPE(args->data[0]) == Lisp_Int) {
        r_int = GET_IVAL(args->data[0]);
    } else if (GET_TYPE(args->data[0]) == Lisp_Float) {
        r_double = GET_FVAL_VAL(args->data[0]);
        float_p = true;
    } else {
        return LISP_ERROR(Evaluation_Error);
    }

    for (size_t i = 1; i < args->size; i++) {
        if (GET_TYPE(args->data[i]) == Lisp_Int) {
            if (float_p == true) {
                r_double -= (double)GET_IVAL(args->data[i]);
            } else {
                r_int -= GET_IVAL(args->data[i]);
            }
        } else if (GET_TYPE(args->data[i]) == Lisp_Float) {
            if (float_p == true) {
                r_double -= GET_FVAL_VAL(args->data[i]);
            } else {
                r_double = (double)r_int;
                r_double -= GET_FVAL_VAL(args->data[i]);
                float_p = true;
            }
        } else {
            return LISP_ERROR(Evaluation_Error);
        }
    }

    if (float_p == true) {
#if DOUBLE_IMMEDIATE
        return LISP_FLOAT(r_double);
#else
        double *flt = cdouble2float(env->mempool, r_double);
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
#endif
    } else {
        return LISP_INT(r_int);
    }

}

Lisp_Object f_minus(NArray *args, lispenv_t *env) {
    if (args->size == 0) {
        /* (-) -> 0 */
        return LISP_INT(0);
    } else if (args->size == 1) {
        return minus_onearg(args, env);
    } else {
#if DOUBLE_IMMEDIATE
        return minus_multiarg(args);
#else
        return minus_multiarg(args, env);
#endif
    }
}

/*
    MARK: Function: car
*/

Lisp_Object f_car(NArray *args, __attribute__((unused)) lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    if (GET_TYPE(args->data[0]) == Lisp_CList) {
        if (GET_AVAL(args->data[0])->size == 0) {
            return LISP_NIL;
        } else {
            return GET_AVAL(args->data[0])->data[0];
        }
    } else if (GET_TYPE(args->data[0]) == Lisp_Nil) {
        return LISP_NIL;
    } else {
        return LISP_ERROR(Evaluation_Error);
    }
}

/*
    MARK: Function: concat
*/

Lisp_Object f_concat(NArray *args, lispenv_t *env) {
    if (args->size == 0) {
        /* returns empty string */
        char *str = copy_to_string_area(env->mempool, "");
        CHECK_ALLOC(str);
        return LISP_STRING(str);
    } else if (args->size == 1) {
        /* returns first argument itself */
        CHECK_TYPE(args->data[0], Lisp_String);
        return args->data[0];
    } else {
        /* concatenate strings */
        size_t len = 0;
        for (size_t i = 0; i < args->size; i++) {
            CHECK_TYPE(args->data[i], Lisp_String);
            len += strlen(GET_SVAL(args->data[i]));
        }
        char *newstr = new_string_area(env->mempool, len + 1);
        CHECK_ALLOC(newstr);
        char *p = newstr;
        for (size_t i = 0; i < args->size; i++) {
            char *src = GET_SVAL(args->data[i]);
            while (*src != '\0') {
                *p++ = *src++;
            }
        }
        *p = '\0';
        return LISP_STRING(newstr);
    }
}

/*
    MARK: Function: make-string
*/

Lisp_Object f_make_string(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 2);
    CHECK_TYPE(args->data[0], Lisp_Int);
    CHECK_TYPE(args->data[1], Lisp_Int);
    int n = GET_IVAL(args->data[0]);
    CHECK_CONDITION(n >= 0);
    char c = GET_IVAL(args->data[1]);
    char *str = new_string_area(env->mempool, n + 1);
    CHECK_ALLOC(str);
    for (size_t i = 0; i < (size_t)n; i++) {
        str[i] = c;
    }
    str[n] = '\0';
    return LISP_STRING(str);
}

/*
    MARK: Function: string-to-char
*/

Lisp_Object f_string_to_char(NArray *args, __attribute__((unused)) lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_String);
    char *str = GET_SVAL(args->data[0]);
    return LISP_INT(str[0]);
}

/*
    MARK: Function: symbol-value
*/

Lisp_Object f_symbol_value(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_Symbol);
    Lisp_Object val = get_variable(env->variable_pool, GET_SVAL(args->data[0]));
    if (GET_TYPE(val) == Dynamic_Val) {
        /* dynamic variable */
        Lisp_Object (*fn)(lispenv_t *) = GET_FNVAL(val);
        val = fn(env);
    }
    return val;
}

/*
    MARK: Function: substring
*/

Lisp_Object f_substring(NArray *args, lispenv_t *env) {
    CHECK_CONDITION(args->size == 3);
    CHECK_TYPE(args->data[0], Lisp_String);
    CHECK_TYPE(args->data[1], Lisp_Int);
    CHECK_TYPE(args->data[2], Lisp_Int);

    char *str = GET_SVAL(args->data[0]);
    size_t slen = strlen(str);
    int from = GET_IVAL(args->data[1]);
    int to = GET_IVAL(args->data[2]);
    CHECK_CONDITION(0 <= from && from <= (int)slen);
    CHECK_CONDITION(from <= to && to <= (int)slen);

    size_t newlen = to - from;
    char *newstr = new_string_area(env->mempool, newlen + 1);
    CHECK_ALLOC(newstr);
    for (size_t i = 0; i < newlen; i++) {
        newstr[i] = str[from + i];
    }
    newstr[newlen] = '\0';
    return LISP_STRING(newstr);
}

/*
    MARK: Function: string-to-number
*/

#if DOUBLE_IMMEDIATE
Lisp_Object f_string_to_number(NArray *args, __attribute__((unused)) lispenv_t *env) {
#else
Lisp_Object f_string_to_number(NArray *args, lispenv_t *env) {
#endif
    CHECK_CONDITION(args->size == 1);
    CHECK_TYPE(args->data[0], Lisp_String);
    char *str = GET_SVAL(args->data[0]);
    check_num_t result = check_num_str(str, false);
    if (result == R_INT) {
        /* integer*/
        errno = 0;
        int n = (int)strtol(str, NULL, 10);
        CHECK_CONDITION(errno == 0);
        return LISP_INT(n);
    } else if (result == R_FLOAT) {
        /* float */
        double fnum = strtod(str, NULL);
#if DOUBLE_IMMEDIATE
        return LISP_FLOAT(fnum);
#else
        double *flt = cdouble2float(env->mempool, fnum);
        CHECK_ALLOC(flt);
        return LISP_FLOAT(flt);
#endif
    } else {
        /* not number*/
        return LISP_INT(0);
    }
}

/*
    MARK: Function: pwd
*/

Lisp_Object get_pwd_str(lispenv_t *env) {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer))) {
        char *path = copy_to_string_area(env->mempool, buffer);
        CHECK_ALLOC(path);
        return LISP_STRING(path);
    } else {
        return LISP_ERROR(Evaluation_Error);
    }
}

#if EXPOSE_SYSTEM_ENV
Lisp_Object f_pwd( __attribute__((unused)) NArray *args, lispenv_t *env) {
    return get_pwd_str(env);
}
#else /* ! EXPOSE_SYSTEM_ENV*/
Lisp_Object f_pwd( __attribute__((unused)) NArray *args, __attribute__((unused)) lispenv_t *env) {
    return LISP_STRING(V_CWD);
}
#endif /* EXPOSE_SYSTEM_ENV*/

/*
    MARK: Function: window-width
*/

Lisp_Object f_window_width( __attribute__((unused)) NArray *args, __attribute__((unused)) lispenv_t *env) {
    return LISP_INT(V_WINDOW_WIDTH);
}

/*
    MARK: Function: current-time-string
*/

Lisp_Object f_current_time_string(NArray *args, lispenv_t *env) {
    struct tm newtime;
    char buffer[STRFTIME_BUFFSIZE];

    CHECK_CONDITION(args->size == 0);
    env->current_time_func(&newtime);
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", &newtime);
    char *str = copy_to_string_area(env->mempool, buffer);
    CHECK_ALLOC(str);
    return LISP_STRING(str);
}

/*
    dynamic variables.
    I suppose these variables are rarely used.
    So, it is not efficient to set these variables every time interpreter is started.
*/

Lisp_Object dv_user_full_name(lispenv_t *env) {
    struct passwd pwd_data;
    struct passwd *result;
    char buffer[GETPWUID_R_BUFFSIZE];
    uid_t uid = getuid();

    int rtn = getpwuid_r(uid, &pwd_data, buffer, sizeof(buffer), &result);
    CHECK_CONDITION(rtn == 0 && result != NULL);
    char *comma_ptr = strchr(pwd_data.pw_gecos, ',');
    if (comma_ptr) {
        *comma_ptr = '\0';
    }
    char *str = copy_to_string_area(env->mempool, pwd_data.pw_gecos);
    CHECK_ALLOC(str);
    return LISP_STRING(str);
}

Lisp_Object dv_user_mail_address(lispenv_t *env) {
    char buff1[LOGIN_NAME_MAX + 1];
    if (getlogin_r(buff1, sizeof(buff1)) != 0) {
        strcpy(buff1, DUMMY_USER_NAME);
    }

    char buff2[HOST_NAME_MAX + 1];
    CHECK_CONDITION(gethostname(buff2, sizeof(buff2)) == 0);

    size_t len = strlen(buff1) + strlen(buff2) + 2;
    char *str = new_string_area(env->mempool, len);
    CHECK_ALLOC(str);
    if (snprintf(str, len, "%s@%s", buff1, buff2) >= (int)len) {
        return LISP_ERROR(Memory_Error);
    }
    return LISP_STRING(str);
}

/*
    MARK: register functions and variables
*/

int register_func_simple(lispenv_t *env) {
    func_pool_t *func_pool = env->func_pool;
    ADD_FUNC_OR_RETURN(func_pool, "-", f_minus);
    ADD_FUNC_OR_RETURN(func_pool, "car", f_car);
    ADD_FUNC_OR_RETURN(func_pool, "concat", f_concat);
    ADD_FUNC_OR_RETURN(func_pool, "make-string", f_make_string);
    ADD_FUNC_OR_RETURN(func_pool, "string-to-char", f_string_to_char);
    ADD_FUNC_OR_RETURN(func_pool, "symbol-value", f_symbol_value);
    ADD_FUNC_OR_RETURN(func_pool, "substring", f_substring);
    ADD_FUNC_OR_RETURN(func_pool, "string-to-number", f_string_to_number);
    ADD_FUNC_OR_RETURN(func_pool, "pwd", f_pwd);
    ADD_FUNC_OR_RETURN(func_pool, "window-width", f_window_width);
    ADD_FUNC_OR_RETURN(func_pool, "current-time-string", f_current_time_string);

    variable_pool_t *variable_pool = env->variable_pool;
    SET_VARIABVLE_OR_RETURN(variable_pool, "fill-column", LISP_INT(V_FILL_COLUMN));
    SET_VARIABVLE_OR_RETURN(variable_pool, "comment-start", LISP_STRING(V_COMMENT_START));

#if EXPOSE_SYSEM_ENV
    SET_VARIABVLE_OR_RETURN(variable_pool, "user-full-name", DYNAMIC_VAL(dv_user_full_name));
    SET_VARIABVLE_OR_RETURN(variable_pool, "user-mail-address", DYNAMIC_VAL(dv_user_mail_address));
#else
    SET_VARIABVLE_OR_RETURN(variable_pool, "user-full-name", LISP_STRING(V_USER_FULL_NAME));
    SET_VARIABVLE_OR_RETURN(variable_pool, "user-mail-address", LISP_STRING(V_USER_MAIL_ADDRESS));
#endif

    return 0;
}
