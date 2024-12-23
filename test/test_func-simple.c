/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include <time.h>
#include "../src/reader.h"
#include "../src/eval.h"
#include "../src/func-simple.h"

#define TMP_BUFFSIZE 256

static lispenv_t *lisp_env;

int init_for_func_simple_test(void) {
    lisp_env =init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    }

    if (register_func_simple(lisp_env) != 0) {
        end_lispenv(lisp_env);
        return -1;
    }
    return 0;
}

int end_for_func_simple_test(void) {
    end_lispenv(lisp_env);
    return 0;
}

/*
    Function: -
 */

void test_simple_func_minus_int(void) {
    Lisp_Object result1 = eval_expr(reader("(- 5 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 2);

    Lisp_Object result2 = eval_expr(reader("(- 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result2), -5);

    Lisp_Object result3 = eval_expr(reader("(-)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result3), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result3), 0);

    Lisp_Object result4 = eval_expr(reader("(- 10 3 2)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result4), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result4), 5);

    Lisp_Object result5 = eval_expr(reader("(- 3 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result5), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result5), -2);
}

void test_simple_func_minus_float(void) {
    Lisp_Object result1 = eval_expr(reader("(- 5.0 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(GET_FVAL_VAL(result1), 2.0, 1e-10);

    Lisp_Object result2 = eval_expr(reader("(- 5 3.0)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result2), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(GET_FVAL_VAL(result2), 2.0, 1e-10);

    Lisp_Object result3 = eval_expr(reader("(- 3.0 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result3), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(GET_FVAL_VAL(result3), -2.0, 1e-10);
}

void testsuite_simple_func_minus(void) {
    CU_pSuite suite = CU_add_suite("simple-func minus", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func minus int", test_simple_func_minus_int);
    CU_add_test(suite, "simple-func minus float", test_simple_func_minus_float);
}

/*
    Function: car
 */

void test_simple_func_car_list(void) {
    Lisp_Object result = eval_expr(reader("(car '(a b c))", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_Symbol);
    char *sym = str2symbol(lisp_env->symbol_pool, "a", true);
    CU_ASSERT_PTR_EQUAL(GET_SVAL(result), sym);
}

void test_simple_func_car_nil(void) {
    Lisp_Object result1 = eval_expr(reader("(car nil)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Nil);

    Lisp_Object result2 = eval_expr(reader("(car '())", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_Nil);
}

void test_simple_func_car_nonlist(void) {
    Lisp_Object result = eval_expr(reader("(car 'a)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
}

void testsuite_simple_func_car(void) {
    CU_pSuite suite = CU_add_suite("simple-func car", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func car list", test_simple_func_car_list);
    CU_add_test(suite, "simple-func car nil", test_simple_func_car_nil);
    CU_add_test(suite, "simple-func car nonlist", test_simple_func_car_list);
}

/*
    Function: concat
*/

void test_simple_func_concat_noarg(void) {
    Lisp_Object result = eval_expr(reader("(concat)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "");
}

void test_simple_func_concat_onearg(void) {
    Lisp_Object result = eval_expr(reader("(concat \"foo\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "foo");
}

void test_simple_func_concat_multiarg(void) {
    Lisp_Object result = eval_expr(reader("(concat \"foo\" \"bar\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "foobar");
}

void testsuite_simple_func_concat(void) {
    CU_pSuite suite = CU_add_suite("simple-func concat", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func concat noarg", test_simple_func_concat_noarg);
    CU_add_test(suite, "simple-func concat onearg", test_simple_func_concat_onearg);
    CU_add_test(suite, "simple-func concat multiarg", test_simple_func_concat_multiarg);
}

/*
    Function: make-string
*/

void test_simple_func_make_string_normal(void) {
    Lisp_Object result1 = eval_expr(reader("(make-string 3 ?x)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "xxx");

    Lisp_Object result2 = eval_expr(reader("(make-string 0 ?x)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "");
}

void test_simple_func_make_string_error(void) {
    Lisp_Object result1 = eval_expr(reader("(make-string)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Internal_Error);

    Lisp_Object result2 = eval_expr(reader("(make-string 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Internal_Error);

    Lisp_Object result3 = eval_expr(reader("(make-string 3 3 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result3), Internal_Error);

    Lisp_Object result4 = eval_expr(reader("(make-string nil ?x)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result4), Internal_Error);

    Lisp_Object result5 = eval_expr(reader("(make-string 3 nil)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result5), Internal_Error);
}

void testsuite_simple_func_make_string(void) {
    CU_pSuite suite = CU_add_suite("simple-func make-string", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func make-string normal", test_simple_func_make_string_normal);
    CU_add_test(suite, "simple-func make-string error", test_simple_func_make_string_error);
}

/*
    Function: string-to-char
*/

void test_simple_func_string_to_char(void) {
    Lisp_Object result1 = eval_expr(reader("(string-to-char \"abc\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 'a');

    Lisp_Object result2 = eval_expr(reader("(string-to-char \"\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result2), 0);
}

void testsuite_simple_func_string_to_char(void) {
    CU_pSuite suite = CU_add_suite("simple-func string-to-char", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func string-to-char normal", test_simple_func_string_to_char);
}

/*
    Function: symbol-value
*/

void test_simple_func_symbol_value_found(void) {
    CU_ASSERT(set_variable_from_cstr(lisp_env->variable_pool, "foo", LISP_INT(33), true) == 0);

    Lisp_Object result = eval_expr(reader("(symbol-value 'foo)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result), 33);
}

void test_simple_func_symbol_value_notfound(void) {
    Lisp_Object result = eval_expr(reader("(symbol-value 'xxx-non-existent-variable)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
}

static Lisp_Object dummy_dynamic_var_func(__attribute__((unused)) lispenv_t *env) {
    return LISP_INT(52);
}

void test_simple_func_symbol_value_dynamic(void) {
   set_variable_from_cstr(lisp_env->variable_pool, "bar", DYNAMIC_VAL(dummy_dynamic_var_func), true);
    Lisp_Object result = eval_expr(reader("(symbol-value 'bar)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result), 52);
}

void testsuite_simple_func_symbol_value(void) {
    CU_pSuite suite = CU_add_suite("simple-func symbol-value", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func symbol-value found", test_simple_func_symbol_value_found);
    CU_add_test(suite, "simple-func symbol-value fnotound", test_simple_func_symbol_value_notfound);
    CU_add_test(suite, "simple-func symbol-value dynamic", test_simple_func_symbol_value_dynamic);
}

/*
    Function: substring
*/

void test_simple_func_substring_head(void) {
    Lisp_Object result1 = eval_expr(reader("(substring \"abcdef\" 0 2)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "ab");

    Lisp_Object result2 = eval_expr(reader("(substring \"abcdef\" 0 0)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "");
}

void test_simple_func_substring_middle(void) {
    Lisp_Object result = eval_expr(reader("(substring \"abcdef\" 1 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "bc");
}

void test_simple_func_substring_tail(void) {
    Lisp_Object result1 = eval_expr(reader("(substring \"abcdef\" 4 6)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "ef");

    Lisp_Object result2 = eval_expr(reader("(substring \"abcdef\" 6 6)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "");
}

void test_simple_func_substring_error(void) {
    Lisp_Object result1 = eval_expr(reader("(substring \"abc\" 2 1)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Internal_Error);

    Lisp_Object result2 = eval_expr(reader("(substring \"abc\" 2 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Internal_Error);

    Lisp_Object result3 = eval_expr(reader("(substring \"abc\" 5 6)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result3), Internal_Error);
}

void testsuite_simple_func_substring(void) {
    CU_pSuite suite = CU_add_suite("simple-func substring", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func substring head", test_simple_func_substring_head);
    CU_add_test(suite, "simple-func substring middle", test_simple_func_substring_middle);
    CU_add_test(suite, "simple-func substring tail", test_simple_func_substring_tail);
    CU_add_test(suite, "simple-func substring error", test_simple_func_substring_error);
}

/*
    Function: string-to-number
*/

void test_simple_func_string_to_number_int(void) {
    Lisp_Object result1 = eval_expr(reader("(string-to-number \"32\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 32);

    Lisp_Object result2 = eval_expr(reader("(string-to-number \"-32\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result2), -32);
}

void test_simple_func_string_to_number_float(void) {
    Lisp_Object result1 = eval_expr(reader("(string-to-number \"3.2\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(GET_FVAL_VAL(result1), 3.2, 1e-10);
}

void test_simple_func_string_to_number_notnumber(void) {
    Lisp_Object result1 = eval_expr(reader("(string-to-number \"xyz\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 0);
}

void testsuite_simple_func_string_to_number(void) {
    CU_pSuite suite = CU_add_suite("simple-func string-to-number", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func string-to-number int", test_simple_func_string_to_number_int);
    CU_add_test(suite, "simple-func string-to-number float", test_simple_func_string_to_number_float);
    CU_add_test(suite, "simple-func string-to-number notnumber", test_simple_func_string_to_number_notnumber);
}

/*
    Function: pwd
*/

void test_simple_func_pwd_call(void) {
    Lisp_Object result = eval_expr(reader("(pwd)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
}

void test_simple_func_pwd_body(void) {
    Lisp_Object result = get_pwd_str(lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);

    char buffer[TMP_BUFFSIZE];
    CU_ASSERT_PTR_NOT_NULL_FATAL(getcwd(buffer, TMP_BUFFSIZE));
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), buffer);
}

void testsuite_simple_func_pwd(void) {
    CU_pSuite suite = CU_add_suite("simple-func pwd", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func pwd call", test_simple_func_pwd_call);
    CU_add_test(suite, "simple-func pwd body", test_simple_func_pwd_body);
}

/*
    Function: window_width
*/

void test_simple_func_window_width_call(void) {
    Lisp_Object result = eval_expr(reader("(window-width)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Int);
}

void testsuite_simple_func_window_width(void) {
    CU_pSuite suite = CU_add_suite("simple-func window_width", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func window_width call", test_simple_func_window_width_call);
}

/*
    Function: current-time-string
*/

static void dummy_tm(struct tm *tm_ptr) {
    tm_ptr->tm_year = 2023 - 1900;
    tm_ptr->tm_mon = 12 - 1;
    tm_ptr->tm_mday = 31;
    tm_ptr->tm_wday = 0;
    tm_ptr->tm_hour = 13;
    tm_ptr->tm_min = 21;
    tm_ptr->tm_sec = 45;
    tm_ptr->tm_isdst = 0;
}

void test_simple_func_current_time_string_call(void) {
    void (*saved_func)() = lisp_env->current_time_func;
    lisp_env->current_time_func = dummy_tm;
    Lisp_Object result = eval_expr(reader("(current-time-string)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result),  "Sun Dec 31 13:21:45 2023");
    lisp_env->current_time_func = saved_func;
}

void testsuite_simple_func_current_time_string(void) {
    CU_pSuite suite = CU_add_suite("simple-func current_time_string", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func pwd call", test_simple_func_current_time_string_call);
}

/*
    Variables
*/

void test_simple_func_predefined_variables1(void) {
    Lisp_Object result1 = eval_expr(reader("fill-column", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);

    Lisp_Object result2 = eval_expr(reader("comment-start", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result2), Lisp_String);

    Lisp_Object result3 = eval_expr(reader("user-full-name", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result3), Lisp_String);

    Lisp_Object result4 = eval_expr(reader("user-mail-address", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result4), Lisp_String);
}

void testsuite_simple_func_predefined_variables(void) {
    CU_pSuite suite = CU_add_suite("simple-func predefined variables", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func predefined variables", test_simple_func_predefined_variables1);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_simple_func_minus();
    testsuite_simple_func_car();
    testsuite_simple_func_concat();
    testsuite_simple_func_make_string();
    testsuite_simple_func_string_to_char();
    testsuite_simple_func_symbol_value();
    testsuite_simple_func_substring();
    testsuite_simple_func_string_to_number();
    testsuite_simple_func_pwd();
    testsuite_simple_func_window_width();
    testsuite_simple_func_current_time_string();
    testsuite_simple_func_predefined_variables();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
