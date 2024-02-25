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
    CU_ASSERT_EQUAL(result1.type, Lisp_Int);
    CU_ASSERT_EQUAL(result1.val.ival, 2);

    Lisp_Object result2 = eval_expr(reader("(- 5)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(result2.type, Lisp_Int);
    CU_ASSERT_EQUAL(result2.val.ival, -5);

    Lisp_Object result3 = eval_expr(reader("(-)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(result3.type, Lisp_Int);
    CU_ASSERT_EQUAL(result3.val.ival, 0);

    Lisp_Object result4 = eval_expr(reader("(- 10 3 2)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(result4.type, Lisp_Int);
    CU_ASSERT_EQUAL(result4.val.ival, 5);
}

void test_simple_func_minus_float(void) {
    Lisp_Object result1 = eval_expr(reader("(- 5.0 3)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(result1.type, Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(*(result1.val.fval), 2.0, 1e-10);

    Lisp_Object result2 = eval_expr(reader("(- 5 3.0)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(result2.type, Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(*(result2.val.fval), 2.0, 1e-10);
}

void testsuite_simple_func_minus(void) {
    CU_pSuite suite = CU_add_suite("simplt-func minus", init_for_func_simple_test, end_for_func_simple_test);
    CU_add_test(suite, "simple-func minus int", test_simple_func_minus_int);
    CU_add_test(suite, "simple-func minus float", test_simple_func_minus_float);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_simple_func_minus();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
