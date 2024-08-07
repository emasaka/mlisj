/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/reader.h"
#include "../src/eval.h"
#include "helper/writer.h"

#define TMP_BUFFSIZE 128

static lispenv_t *lisp_env;

int init_for_evaltest(void) {
    lisp_env =init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    } else {
        return 0;
    }
}

int end_for_evaltest(void) {
    end_lispenv(lisp_env);
    return 0;
}

/*
  atoms
 */

void test_eval_number(void) {
    Lisp_Object result1 = eval_expr(reader("32", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 32);
}

void test_eval_string(void) {
    Lisp_Object result1 = eval_expr(reader("\"foo\"", lisp_env), lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "\"foo\"");
}

void test_eval_nil(void) {
    Lisp_Object result1 = eval_expr(reader("nil", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Nil);
}

void testsuite_eval_atom(void) {
    CU_pSuite suite = CU_add_suite("eval atom", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval number", test_eval_number);
    CU_add_test(suite, "eval string", test_eval_number);
    CU_add_test(suite, "eval nil", test_eval_nil);
}

/*
  symbol
*/

void test_eval_symbol(void) {
    set_variable_from_cstr(lisp_env->variable_pool, "foo", LISP_INT(32), true);
    Lisp_Object result1 = eval_expr(reader("foo", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result1), 32);
}

void test_eval_keyword_symbol(void) {
    Lisp_Object sym = reader(":foo", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(sym), Lisp_Symbol);
    Lisp_Object result = eval_expr(sym, lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(result), Lisp_Symbol);
    CU_ASSERT_PTR_EQUAL(GET_SVAL(result), GET_SVAL(sym));
}

void testsuite_eval_symbol(void) {
    CU_pSuite suite = CU_add_suite("eval symbol", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval symbol", test_eval_symbol);
    CU_add_test(suite, "eval keyword symbol", test_eval_keyword_symbol);
}

/*
  special form
 */

void test_eval_quote(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = eval_expr(reader("'foo", lisp_env), lisp_env);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "foo");

    Lisp_Object result2 = eval_expr(reader("(quote bar)", lisp_env), lisp_env);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "bar");
}

void test_eval_lambda(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object result1 = eval_expr(reader("(lambda (x))", lisp_env), lisp_env);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(lambda (x))");
}

void testsuite_eval_specialform(void) {
    CU_pSuite suite = CU_add_suite("eval specialform", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval quote", test_eval_quote);
    CU_add_test(suite, "eval lambda", test_eval_lambda);
}

/*
  function call
 */

void test_eval_func_call_notexist(void) {
    Lisp_Object result = eval_expr(reader("(xxx-non-existent-function)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
    CU_ASSERT_EQUAL(GET_ERROR_TYPE(result), Evaluation_Error);
}

void testsuite_eval_func_call(void) {
    CU_pSuite suite = CU_add_suite("eval function call", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval non-existent function call", test_eval_func_call_notexist);
}

/*
  dynamic variable
 */

static Lisp_Object dummy_dynamic_var_func(__attribute__((unused)) lispenv_t *env) {
    return LISP_INT(52);
}

void test_eval_dynamic_var_call(void) {
    set_variable_from_cstr(lisp_env->variable_pool, "foo", DYNAMIC_VAL(dummy_dynamic_var_func), true);
    Lisp_Object result = eval_expr(reader("foo", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result), 52);
}

void testsuite_eval_dynamic_variable(void) {
    CU_pSuite suite = CU_add_suite("eval dynamic variable", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval dynamic variable call", test_eval_dynamic_var_call);
}

/*
  call lambda
 */

void test_eval_call_lambda_call(void) {
    Lisp_Object lambda_lst = reader("(lambda (x) x)", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(lambda_lst), Lisp_CList);
    Lisp_Object args = reader("(5)", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(args), Lisp_CList);
    Lisp_Object result = call_lambda(lambda_lst, GET_AVAL(args), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(result), 5);
}

void test_eval_call_lambda_restore_var(void) {
    char *sym_x = str2symbol(lisp_env->symbol_pool, "x", true);
    Lisp_Object result = get_variable(lisp_env->variable_pool, sym_x);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
}

void testsuite_eval_call_lambda(void) {
    CU_pSuite suite = CU_add_suite("eval call_lambda", init_for_evaltest, end_for_evaltest);
    CU_add_test(suite, "eval call_lambda call", test_eval_call_lambda_call);
    CU_add_test(suite, "eval call_lambda restore variable", test_eval_call_lambda_restore_var);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_eval_atom();
    testsuite_eval_symbol();
    testsuite_eval_specialform();
    testsuite_eval_func_call();
    testsuite_eval_dynamic_variable();
    testsuite_eval_call_lambda();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
