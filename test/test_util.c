#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/util.h"

/*
  check_num_str
 */

void test_check_num_str_int(void) {
    CU_ASSERT_EQUAL(check_num_str("32", true), R_INT);
    CU_ASSERT_EQUAL(check_num_str("-32", true), R_INT);
}

void test_check_num_str_float(void) {
    CU_ASSERT_EQUAL(check_num_str("3.2", true), R_FLOAT);
    CU_ASSERT_EQUAL(check_num_str(".3", true), R_FLOAT);
    CU_ASSERT_EQUAL(check_num_str("3.", true), R_FLOAT);
}

void test_check_num_str_with_nonnumber(void) {
    CU_ASSERT_EQUAL(check_num_str("32x", true), R_NOTNUM);
    CU_ASSERT_EQUAL(check_num_str("32x", false), R_INT);
}

void testsuite_check_num_str(void) {
    CU_pSuite suite = CU_add_suite("test check_num_str", NULL, NULL);
    CU_add_test(suite, "check_num_str int", test_check_num_str_int);
    CU_add_test(suite, "check_num_str float", test_check_num_str_float);
    CU_add_test(suite, "check_num_str with nonnumber", test_check_num_str_with_nonnumber);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_check_num_str();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
