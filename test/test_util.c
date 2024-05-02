#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/util.h"

#define TMP_BUFFSIZE 128

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
    skk_num_type1_kanji
*/

void test_skk_num_type1_kanji(void) {
    char buff[TMP_BUFFSIZE];
    skk_num_type1_kanji("123", buff, TMP_BUFFSIZE);
    CU_ASSERT_STRING_EQUAL(buff, "１２３");
}

void testsuite_skk_num_type1_kanji(void) {
    CU_pSuite suite = CU_add_suite("test skk_num_type1_kanji", NULL, NULL);
    CU_add_test(suite, "skk_num_type1_kanji", test_skk_num_type1_kanji);
}

/*
    skk_num_type3_kanji
*/

void test_skk_num_type3_kanji_zero(void) {
    char buff[TMP_BUFFSIZE];
    int rtn = skk_num_type3_kanji("0", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "〇");
}

void test_skk_num_type3_kanji_1digit(void) {
    char buff[TMP_BUFFSIZE];
    int rtn = skk_num_type3_kanji("3", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "三");
}

void test_skk_num_type3_kanji_2digits(void) {
    char buff[TMP_BUFFSIZE];

    int rtn1 = skk_num_type3_kanji("10", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn1, 0);
    CU_ASSERT_STRING_EQUAL(buff, "十");

    int rtn2 = skk_num_type3_kanji("20", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn2, 0);
    CU_ASSERT_STRING_EQUAL(buff, "二十");

    int rtn3 = skk_num_type3_kanji("52", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn3, 0);
    CU_ASSERT_STRING_EQUAL(buff, "五十二");
}

void test_skk_num_type3_kanji_under_3digits(void) {
    char buff[TMP_BUFFSIZE];

    int rtn1 = skk_num_type3_kanji("100", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn1, 0);
    CU_ASSERT_STRING_EQUAL(buff, "百");

    int rtn2 = skk_num_type3_kanji("211", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn2, 0);
    CU_ASSERT_STRING_EQUAL(buff, "二百十一");

    int rtn3 = skk_num_type3_kanji("205", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn3, 0);
    CU_ASSERT_STRING_EQUAL(buff, "二百五");
}

void test_skk_num_type3_kanji_under_4digits(void) {
    char buff[TMP_BUFFSIZE];

    int rtn1 = skk_num_type3_kanji("1000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn1, 0);
    CU_ASSERT_STRING_EQUAL(buff, "千");

    int rtn2 = skk_num_type3_kanji("3456", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn2, 0);
    CU_ASSERT_STRING_EQUAL(buff, "三千四百五十六");
}

void test_skk_num_type3_kanji_man(void) {
    char buff[TMP_BUFFSIZE];

    int rtn1 = skk_num_type3_kanji("10000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn1, 0);
    CU_ASSERT_STRING_EQUAL(buff, "一万");

    int rtn2 = skk_num_type3_kanji("34567", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn2, 0);
    CU_ASSERT_STRING_EQUAL(buff, "三万四千五百六十七");
}

void test_skk_num_type3_kanji_under_10mil(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_type3_kanji("10000000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "一千万");
}

void test_skk_num_type3_kanji_oku(void) {
    char buff[TMP_BUFFSIZE];

    int rtn1 = skk_num_type3_kanji("100000000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn1, 0);
    CU_ASSERT_STRING_EQUAL(buff, "一億");

    int rtn2 = skk_num_type3_kanji("567000000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn2, 0);
    CU_ASSERT_STRING_EQUAL(buff, "五億六千七百万");
}

void test_skk_num_type3_kanji_cho(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_type3_kanji("1000000000000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "一兆");
}

void test_skk_num_type3_kanji_kei(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_type3_kanji("10000000000000000", buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "一京");
}

void testsuite_skk_num_type3_kanji(void) {
    CU_pSuite suite = CU_add_suite("test skk_num_type3_kanji", NULL, NULL);
    CU_add_test(suite, "skk_num_type3_kanji zero", test_skk_num_type3_kanji_zero);
    CU_add_test(suite, "skk_num_type3_kanji 1 digit", test_skk_num_type3_kanji_1digit);
    CU_add_test(suite, "skk_num_type3_kanji 2 digits", test_skk_num_type3_kanji_2digits);
    CU_add_test(suite, "skk_num_type3_kanji 3 digits", test_skk_num_type3_kanji_under_3digits);
    CU_add_test(suite, "skk_num_type3_kanji 4 digits", test_skk_num_type3_kanji_under_4digits);
    CU_add_test(suite, "skk_num_type3_kanji man", test_skk_num_type3_kanji_man);
    CU_add_test(suite, "skk_num_type3_kanji 1000man", test_skk_num_type3_kanji_under_10mil);
    CU_add_test(suite, "skk_num_type3_kanji oku", test_skk_num_type3_kanji_oku);
    CU_add_test(suite, "skk_num_type3_kanji cho", test_skk_num_type3_kanji_cho);
    CU_add_test(suite, "skk_num_type3_kanji kei", test_skk_num_type3_kanji_kei);
}

/*
    skk_num_exp
*/

void test_skk_num_exp_type0(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_exp("5432", 0, buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "5432");
}

void test_skk_num_exp_type1(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_exp("5432", 1, buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "５４３２");
}

void test_skk_num_exp_type3(void) {
    char buff[TMP_BUFFSIZE];

    int rtn = skk_num_exp("5432", 3, buff, TMP_BUFFSIZE);
    CU_ASSERT_EQUAL(rtn, 0);
    CU_ASSERT_STRING_EQUAL(buff, "五千四百三十二");
}

void testsuite_skk_num_exp(void) {
    CU_pSuite suite = CU_add_suite("test skk_num_exp", NULL, NULL);
    CU_add_test(suite, "skk_num_exp type0", test_skk_num_exp_type0);
    CU_add_test(suite, "skk_num_exp type1", test_skk_num_exp_type1);
    CU_add_test(suite, "skk_num_exp type3", test_skk_num_exp_type3);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_check_num_str();
    testsuite_skk_num_type1_kanji();
    testsuite_skk_num_type3_kanji();
    testsuite_skk_num_exp();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
