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

/*
  String Pool
 */

void test_string_area_normal(void) {
    char *str1 = new_string_area(16);
    CU_ASSERT_PTR_NOT_NULL(str1);
    char *str2 = new_string_area(16);
    CU_ASSERT(str1 != str2);
}

void test_string_area_edge(void) {
    string_pool_used = STRING_POOL_SIZE - 1;
    char *str1 = new_string_area(1);
    CU_ASSERT_PTR_NOT_NULL(str1);
    char *str2 = new_string_area(1);
    CU_ASSERT_PTR_NULL(str2);
}

void testsuite_string_area(void) {
    CU_pSuite suite = CU_add_suite("string_area_test", init_mempool, NULL);
    CU_add_test(suite, "string_allocate_normal", test_string_area_normal);
    CU_add_test(suite, "string_allocate_edge", test_string_area_edge);
 }

void test_string_copy_normal(void) {
    char *str_src1 = "abc";
    char *str_src2 = "def";
    char *str_dst1 = copy_to_string_area(str_src1);
    CU_ASSERT_PTR_NOT_NULL(str_dst1);
    CU_ASSERT(str_src1 != str_dst1);
    CU_ASSERT(strcmp(str_src1, str_dst1) == 0);
    char *str_dst2 = copy_to_string_area(str_src2);
    CU_ASSERT(str_dst1 != str_dst2);
}

void test_string_copy_edge(void) {
    char *str_src1 = "ab";
    char *str_src2 = "cd";
    char *str_src3 = "de";
    string_pool_used = STRING_POOL_SIZE - (strlen(str_src1) + 1) - (strlen(str_src2) + 1);
    char *str_dst1 = copy_to_string_area(str_src1);
    CU_ASSERT_PTR_NOT_NULL(str_dst1);
    char *str_dst2 = copy_to_string_area(str_src2);
    CU_ASSERT_PTR_NOT_NULL(str_dst2);
    char *str_dst3 = copy_to_string_area(str_src3);
    CU_ASSERT_PTR_NULL(str_dst3);
}

void testsuite_string_copy(void) {
    CU_pSuite suite = CU_add_suite("string_copy_test", init_mempool, NULL);
    CU_add_test(suite, "string_copy_normal", test_string_copy_normal);
    CU_add_test(suite, "string_copy_edge", test_string_copy_edge);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_lispobject_pool();
    testsuite_string_area();
    testsuite_string_copy();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
