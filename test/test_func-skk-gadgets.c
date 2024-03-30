#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include "../src/reader.h"
#include "../src/eval.h"
#include "../src/func-skk-gadgets.h"

static lispenv_t *lisp_env;

int init_for_func_skk_gadgets_test(void) {
    lisp_env =init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    }

    if (register_func_skk_gadgets(lisp_env) != 0) {
        end_lispenv(lisp_env);
        return -1;
    }
    return 0;
}

int end_for_func_skk_gadgets_test(void) {
    end_lispenv(lisp_env);
    return 0;
}

/*
    Function: skk-version
 */

void test_skk_gadgets_func_skk_version_call(void) {
    Lisp_Object result = eval_expr(reader("(skk-version)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
}

void testsuite_skk_gadgets_func_skk_version(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func skk-verrion", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func skk-version call", test_skk_gadgets_func_skk_version_call);
}

/*
    Dynamic variable: skk-num-list
*/

void test_skk_gadgets_func_skk_num_list_empty(void) {
    register_skk_num_list(lisp_env, NULL);

    Lisp_Object result = eval_expr(reader("skk-num-list", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_Nil);
}

void test_skk_gadgets_func_skk_num_list_notempty(void) {
    char *num_list[] = { "32", "50", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result = eval_expr(reader("skk-num-list", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_CList);
    CU_ASSERT_EQUAL(GET_AVAL(result)->size, 2);

    Lisp_Object elm0 = GET_AVAL(result)->data[0];
    CU_ASSERT_EQUAL(GET_TYPE(elm0), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(elm0), "32");

    Lisp_Object elm1 = GET_AVAL(result)->data[1];
    CU_ASSERT_EQUAL(GET_TYPE(elm1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(elm1), "50");
}

void testsuite_skk_gadgets_func_skk_num_list(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func skk-num-list", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func skk-num-list empty", test_skk_gadgets_func_skk_num_list_empty);
    CU_add_test(suite, "skk-gadgets-func skk-num-list notempty", test_skk_gadgets_func_skk_num_list_notempty);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_skk_gadgets_func_skk_version();
    testsuite_skk_gadgets_func_skk_num_list();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
