#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/mempool.h"
#include "../src/symbol.h"
#include "../src/variable.c"

int init_for_variabletest(void) {
    init_mempool();
    init_symbol();
    init_variables();
    return 0;
}

void test_variable_found(void) {
    char *sym1 = str2symbol("foo", true);
    char *sym2 = str2symbol("bar", true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_PTR_NOT_NULL(sym2);

    CU_ASSERT(set_variable(sym1, (Lisp_Object){ .type = Lisp_Int, .val.ival = 31 }) == 0);
    CU_ASSERT(set_variable(sym2, (Lisp_Object){ .type = Lisp_Nil, .val.ival = 0 }) == 0);

    Lisp_Object val1 = get_variable(sym1);
    CU_ASSERT_EQUAL(val1.type, Lisp_Int);
    CU_ASSERT_EQUAL(val1.val.ival, 31);
    Lisp_Object val2 = get_variable(sym2);
    CU_ASSERT_EQUAL(val2.type, Lisp_Nil);
}

void test_variable_notfound(void) {
    char *sym = str2symbol("baz", true);
    CU_ASSERT_PTR_NOT_NULL(sym);
    Lisp_Object val = get_variable(sym);
    CU_ASSERT_EQUAL(val.type, Internal_Error);
    CU_ASSERT_EQUAL(val.val.err, Variable_Error);
}

void testsuite_variable_set_get(void) {
    CU_pSuite suite = CU_add_suite("variable_set_get", init_for_variabletest, NULL);
    CU_add_test(suite, "variable_found", test_variable_found);
    CU_add_test(suite, "variable_notfound", test_variable_notfound);
}

void test_variable_stack_frame(void) {
    char *sym = str2symbol("hoge", true);
    CU_ASSERT_PTR_NOT_NULL(sym);

    CU_ASSERT(set_variable(sym, (Lisp_Object){ .type = Lisp_Int, .val.ival = 31 }) == 0);
    int st = save_variable_status();
    CU_ASSERT(set_variable(sym, (Lisp_Object){ .type = Lisp_Nil, .val.ival = 0 }) == 0);

    Lisp_Object val1 = get_variable(sym);
    CU_ASSERT_EQUAL(val1.type, Lisp_Nil);

    restore_variable_status(st);
    Lisp_Object val2 = get_variable(sym);
    CU_ASSERT_EQUAL(val2.type, Lisp_Int);
    CU_ASSERT_EQUAL(val2.val.ival, 31);
}

void testsuite_variable_stack_frame(void) {
    CU_pSuite suite = CU_add_suite("variable_stack_frame", init_for_variabletest, NULL);
    CU_add_test(suite, "variable_stack_frame", test_variable_stack_frame);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_variable_set_get();
    testsuite_variable_stack_frame();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
