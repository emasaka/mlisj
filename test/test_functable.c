#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/mempool.h"
#include "../src/symbol.h"
#include "../src/functable.c"

int init_for_functabletest(void) {
    init_mempool();
    init_symbol();
    init_func_table();
    return 0;
}

int dummy_func1(int n) {
    return n + 1;
}

int dummy_func2(int n) {
    return n + 2;
}

void test_functable_found(void) {
    char *sym1 = str2symbol("foo", true);
    char *sym2 = str2symbol("bar", true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_PTR_NOT_NULL(sym2);

    CU_ASSERT(add_func(sym1, dummy_func1) == 0);
    CU_ASSERT(add_func(sym2, dummy_func2) == 0);

    cfunc_t func1 = get_func(sym1);
    CU_ASSERT_PTR_NOT_NULL(func1);
    CU_ASSERT_EQUAL((*func1)(3), 4);

    cfunc_t func2 = get_func(sym2);
    CU_ASSERT_PTR_NOT_NULL(func2);
    CU_ASSERT_EQUAL((*func2)(3), 5);
}

void test_functable_notfound(void) {
    char *sym = str2symbol("baz", true);
    CU_ASSERT_PTR_NOT_NULL(sym);
    CU_ASSERT_PTR_NULL(get_func(sym));
}

void testsuite_functable_set_get(void) {
    CU_pSuite suite = CU_add_suite("functable_set_get", init_for_functabletest, NULL);
    CU_add_test(suite, "functable_found", test_functable_found);
    CU_add_test(suite, "functable_notfound", test_functable_notfound);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_functable_set_get();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
