#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/mempool.c"

/*
  Lisp_Object Pool
 */

void test_lispobject_pool_normal(void) {
    Lisp_Object *obj1 = new_lispobject_array(1);
    CU_ASSERT_PTR_NOT_NULL(obj1);
    Lisp_Object *obj2 = new_lispobject_array(1);
    CU_ASSERT(obj1 != obj2);
}

void test_lispobject_pool_edge(void) {
    lispobject_pool_used = LISPOBJECT_POOL_SIZE - 1;
    Lisp_Object *obj1 = new_lispobject_array(1);
    CU_ASSERT_PTR_NOT_NULL(obj1);
    Lisp_Object *obj2 = new_lispobject_array(1);
    CU_ASSERT_PTR_NULL(obj2);
}

void testsuite_lispobject_pool(void) {
    CU_pSuite suite = CU_add_suite("lispobject_pool_test", init_mempool, NULL);
    CU_add_test(suite, "lispobject_allocate_normal", test_lispobject_pool_normal);
    CU_add_test(suite, "lispobject_allocate_edge", test_lispobject_pool_edge);
}

int main(void) {
    CU_initialize_registry();

    testsuite_lispobject_pool();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
