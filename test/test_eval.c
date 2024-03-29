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
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
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
    CU_ASSERT_EQUAL(GET_TYPE(sym), Lisp_Symbol);
    Lisp_Object result = eval_expr(sym, lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Symbol);
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
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_eval_atom();
    testsuite_eval_symbol();
    testsuite_eval_specialform();
    testsuite_eval_func_call();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
