/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/mempool.h"

static mempool_t *mempool;

int init_for_mempooltest(void) {
    mempool = init_mempool();
    if (mempool == NULL) {
        return -1;
    } else {
        return 0;
    }
}

int end_for_mempooltest(void) {
    end_mempool(mempool);
    return 0;
}

/*
  Lisp_Object Pool
 */

void test_lispobject_pool_normal(void) {
    Lisp_Object *obj1 = new_lispobject_array(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(obj1);
    Lisp_Object *obj2 = new_lispobject_array(mempool, 1);
    CU_ASSERT_PTR_NOT_EQUAL(obj1, obj2);
}

void test_lispobject_pool_edge(void) {
    mempool->lispobject_pool_used = LISPOBJECT_POOL_SIZE - 1;
    Lisp_Object *obj1 = new_lispobject_array(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(obj1);
    Lisp_Object *obj2 = new_lispobject_array(mempool, 1);
    CU_ASSERT_PTR_NULL(obj2);
}

void testsuite_lispobject_pool(void) {
    CU_pSuite suite = CU_add_suite("lispobject_pool_test", init_for_mempooltest, end_for_mempooltest);
    CU_add_test(suite, "lispobject_allocate_normal", test_lispobject_pool_normal);
    CU_add_test(suite, "lispobject_allocate_edge", test_lispobject_pool_edge);
}

/*
  String Pool
 */

void test_string_area_normal(void) {
    char *str1 = new_string_area(mempool, 16);
    CU_ASSERT_PTR_NOT_NULL(str1);
    char *str2 = new_string_area(mempool, 16);
    CU_ASSERT_PTR_NOT_EQUAL(str1, str2);
}

void test_string_area_edge(void) {
    mempool->string_pool_used = STRING_POOL_SIZE - 1;
    char *str1 = new_string_area(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(str1);
    char *str2 = new_string_area(mempool, 1);
    CU_ASSERT_PTR_NULL(str2);
}

void testsuite_string_area(void) {
    CU_pSuite suite = CU_add_suite("string_area_test", init_for_mempooltest, end_for_mempooltest);
    CU_add_test(suite, "string_allocate_normal", test_string_area_normal);
    CU_add_test(suite, "string_allocate_edge", test_string_area_edge);
 }

void test_string_copy_normal(void) {
    char *str_src1 = "abc";
    char *str_src2 = "def";
    char *str_dst1 = copy_to_string_area(mempool, str_src1);
    CU_ASSERT_PTR_NOT_NULL(str_dst1);
    CU_ASSERT_PTR_NOT_EQUAL(str_src1, str_dst1);
    CU_ASSERT_STRING_EQUAL(str_src1, str_dst1);
    char *str_dst2 = copy_to_string_area(mempool, str_src2);
    CU_ASSERT_PTR_NOT_EQUAL(str_dst1, str_dst2);
}

void test_string_copy_edge(void) {
    char *str_src1 = "ab";
    char *str_src2 = "cd";
    char *str_src3 = "de";
    mempool->string_pool_used = STRING_POOL_SIZE - (strlen(str_src1) + 1) - (strlen(str_src2) + 1);
    char *str_dst1 = copy_to_string_area(mempool, str_src1);
    CU_ASSERT_PTR_NOT_NULL(str_dst1);
    char *str_dst2 = copy_to_string_area(mempool, str_src2);
    CU_ASSERT_PTR_NOT_NULL(str_dst2);
    char *str_dst3 = copy_to_string_area(mempool, str_src3);
    CU_ASSERT_PTR_NULL(str_dst3);
}

void testsuite_string_copy(void) {
    CU_pSuite suite = CU_add_suite("string_copy_test", init_for_mempooltest, end_for_mempooltest);
    CU_add_test(suite, "string_copy_normal", test_string_copy_normal);
    CU_add_test(suite, "string_copy_edge", test_string_copy_edge);
}

#if !DOUBLE_IMMEDIATE
/*
  Float Pool
 */

void test_float_pool_normal(void) {
    double *f1 = cdouble2float(mempool, 1.0);
    CU_ASSERT_PTR_NOT_NULL(f1);
    double *f2 = cdouble2float(mempool, 1.0);
    CU_ASSERT_PTR_NOT_EQUAL(f1, f2);
}

void test_float_pool_edge(void) {
    mempool->float_pool_used = FLOAT_POOL_SIZE - 1;
    double *f1 = cdouble2float(mempool, 1.0);
    CU_ASSERT_PTR_NOT_NULL(f1);
    double *f2 = cdouble2float(mempool, 1.0);
    CU_ASSERT_PTR_NULL(f2);
}

void testsuite_float_pool(void) {
    CU_pSuite suite = CU_add_suite("float_pool_test", init_for_mempooltest, end_for_mempooltest);
    CU_add_test(suite, "float_allocate_normal", test_float_pool_normal);
    CU_add_test(suite, "float_allocate_edge", test_float_pool_edge);
}
#endif

/*
  Array Node Pool
 */

void test_narray_node_pool_normail(void) {
    NArray *n1 = new_narray(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(n1);
    CU_ASSERT_EQUAL(n1->size, 1);
    CU_ASSERT_PTR_NOT_NULL(n1->data);
    NArray *n2 = new_narray(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(n2);
    CU_ASSERT_NOT_EQUAL(n1, n2);
}

void test_narray_node_pool_edge(void) {
    mempool->narray_node_pool_used = NARRAY_NODE_POOL_SIZE - 1;
    NArray *n1 = new_narray(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(n1);
    NArray *n2 = new_narray(mempool, 1);
    CU_ASSERT_PTR_NULL(n2);
}

void testsuite_narray_node_pool(void) {
    CU_pSuite suite = CU_add_suite("narray_node_pool_test", init_for_mempooltest, end_for_mempooltest);
    CU_add_test(suite, "narray_node_pool_normal", test_narray_node_pool_normail);
    CU_add_test(suite, "narray_node_pool_edge", test_narray_node_pool_edge);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_lispobject_pool();
    testsuite_string_area();
    testsuite_string_copy();
#if !DOUBLE_IMMEDIATE
    testsuite_float_pool();
#endif
    testsuite_narray_node_pool();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
