#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/reader.h"
#include "../src/eval.h"
#include "../src/func-simple.h"

static lispenv_t *lisp_env;

int init_for_func_simple_test(void) {
    lisp_env =init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    }

    if (register_func_simple(lisp_env->func_pool) != 0) {
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
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(*(GET_FVAL(result1)), 2.0, 1e-10);

    Lisp_Object result2 = eval_expr(reader("(- 5 3.0)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(*(GET_FVAL(result2)), 2.0, 1e-10);

    Lisp_Object result3 = eval_expr(reader("(- 3.0 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result3), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(*(GET_FVAL(result3)), -2.0, 1e-10);
}

void testsuite_simple_func_minus(void) {
    CU_pSuite suite = CU_add_suite("simplt-func minus", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func minus int", test_simple_func_minus_int);
    CU_add_test(suite, "simple-func minus float", test_simple_func_minus_float);
}

/*
    Function: car
 */

void test_simple_func_car_list(void) {
    Lisp_Object result = eval_expr(reader("(car '(a b c))", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Symbol);
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
    CU_pSuite suite = CU_add_suite("simplt-func car", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func car list", test_simple_func_car_list);
    CU_add_test(suite, "simple-func car nil", test_simple_func_car_nil);
    CU_add_test(suite, "simple-func car nonlist", test_simple_func_car_list);
}

/*
    Function: concat
*/

void test_simple_func_concat_noarg(void) {
    Lisp_Object result = eval_expr(reader("(concat)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "");
}

void test_simple_func_concat_onearg(void) {
    Lisp_Object result = eval_expr(reader("(concat \"foo\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "foo");
}

void test_simple_func_concat_multiarg(void) {
    Lisp_Object result = eval_expr(reader("(concat \"foo\" \"bar\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "foobar");
}

void testsuite_simple_func_concat(void) {
    CU_pSuite suite = CU_add_suite("simplt-func concat", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func concat noarg", test_simple_func_concat_noarg);
    CU_add_test(suite, "simple-func concat onearg", test_simple_func_concat_onearg);
    CU_add_test(suite, "simple-func concat multiarg", test_simple_func_concat_multiarg);
}

/*
    Function: make-string
*/

void test_simple_func_make_string_normal(void) {
    Lisp_Object result1 = eval_expr(reader("(make-string 3 ?x)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "xxx");

    Lisp_Object result2 = eval_expr(reader("(make-string 0 ?x)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_String);
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
    CU_pSuite suite = CU_add_suite("simplt-func make-string", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func make-string normal", test_simple_func_make_string_normal);
    CU_add_test(suite, "simple-func make-string error", test_simple_func_make_string_error);
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

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
