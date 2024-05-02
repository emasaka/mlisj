#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include "../src/reader.h"
#include "../src/eval.h"
#include "../src/func-skk-gadgets.h"
#include "helper/writer.h"

#define TMP_BUFFSIZE 128

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
    Helper functions
*/

int string_endswith(char *str, char *ptn) {
    size_t str_size = strlen(str);
    size_t ptn_size = strlen(ptn);
    if (str_size < ptn_size) { return -1; }
    for (size_t i = 1; i <= ptn_size; i++) {
        if (str[str_size - i] != ptn[ptn_size - i]) {
            return -1;
        }
    }
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
    Function: skk-times
*/

void test_skk_gadgets_func_skk_times_zeronums(void) {
    register_skk_num_list(lisp_env, NULL);

    Lisp_Object result = eval_expr(reader("(skk-times)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "1");
}

void test_skk_gadgets_func_skk_times_somenums(void) {
    char *num_list[] = { "3", "5", "7", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result = eval_expr(reader("(skk-times)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result), "105");
}

void testsuite_skk_gadgets_func_skk_times(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func skk-times", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func skk-times zero numbers", test_skk_gadgets_func_skk_times_zeronums);
    CU_add_test(suite, "skk-gadgets-func skk-times zero numbers", test_skk_gadgets_func_skk_times_somenums);
}

/*
    Function: skk-gadget-units-conversion
*/

void test_skk_gadgets_func_units_conversion_mile_km(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"mile\" 2 \"km\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_NSTRING_EQUAL(str, "3.2", 3);
    CU_ASSERT(string_endswith(str, "km") == 0);
}

void test_skk_gadgets_func_units_conversion_mile_yard(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"mile\" 2 \"yard\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_STRING_EQUAL(str, "3520yard");
}

void test_skk_gadgets_func_units_conversion_yard_feet(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"yard\" 2 \"feet\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_STRING_EQUAL(str, "6feet");
}

void test_skk_gadgets_func_units_conversion_yard_cm(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"yard\" 2 \"cm\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_NSTRING_EQUAL(str, "182.88", 6);
    CU_ASSERT(string_endswith(str, "cm") == 0);
}

void test_skk_gadgets_func_units_conversion_feet_inch(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"feet\" 2 \"inch\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_STRING_EQUAL(str, "24inch");
}

void test_skk_gadgets_func_units_conversion_feet_cm(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"feet\" 2 \"cm\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_NSTRING_EQUAL(str, "60.96", 5);
    CU_ASSERT(string_endswith(str, "cm") == 0);
}

void test_skk_gadgets_func_units_conversion_inch_feet(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"inch\" 2 \"feet\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_NSTRING_EQUAL(str, "1", 1);
    CU_ASSERT(string_endswith(str, "feet") == 0);
}

void test_skk_gadgets_func_units_conversion_inch_cm(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"inch\" 2 \"cm\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Lisp_String);
    char *str = GET_SVAL(result);
    CU_ASSERT_NSTRING_EQUAL(str, "5.0", 3);
    CU_ASSERT(string_endswith(str, "cm") == 0);
}

void test_skk_gadgets_func_units_conversion_nomatch(void) {
    Lisp_Object result = eval_expr(reader("(skk-gadget-units-conversion \"km\" 2 \"cm\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
}

void testsuite_skk_gadgets_func_skk_gadget_units_conversion(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func skk-gadget-units-conversion", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion mile->km", test_skk_gadgets_func_units_conversion_mile_km);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion mile->yard", test_skk_gadgets_func_units_conversion_mile_yard);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion yard->feet", test_skk_gadgets_func_units_conversion_yard_feet);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion yard->cm", test_skk_gadgets_func_units_conversion_yard_cm);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion feet->inch", test_skk_gadgets_func_units_conversion_feet_inch);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion feet->cm", test_skk_gadgets_func_units_conversion_feet_cm);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion inch->feet", test_skk_gadgets_func_units_conversion_inch_feet);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion inch->cm", test_skk_gadgets_func_units_conversion_inch_cm);
    CU_add_test(suite, "skk-gadgets-func skk-gadget-units-conversion nomtch", test_skk_gadgets_func_units_conversion_nomatch);
}

/*
    gengo_to_ad_1
*/

void test_skk_gadgets_func_gengo_to_ad_1_reiwa(void) {
    CU_ASSERT_EQUAL(gengo_to_ad_1("れいわ", 6), 2024);
    CU_ASSERT_EQUAL(gengo_to_ad_1("令和", 6), 2024);
}

void test_skk_gadgets_func_gengo_to_ad_1_heisei(void) {
    CU_ASSERT_EQUAL(gengo_to_ad_1("へいせい", 22), 2010);
    CU_ASSERT_EQUAL(gengo_to_ad_1("平成", 22), 2010);
}

void test_skk_gadgets_func_gengo_to_ad_1_shouwa(void) {
    CU_ASSERT_EQUAL(gengo_to_ad_1("しょうわ", 55), 1980);
    CU_ASSERT_EQUAL(gengo_to_ad_1("昭和", 55), 1980);
}

void test_skk_gadgets_func_gengo_to_ad_1_taishou(void) {
    CU_ASSERT_EQUAL(gengo_to_ad_1("たいしょう", 10), 1921);
    CU_ASSERT_EQUAL(gengo_to_ad_1("大正", 10), 1921);
}

void test_skk_gadgets_func_gengo_to_ad_1_meiji(void) {
    CU_ASSERT_EQUAL(gengo_to_ad_1("めいじ", 40), 1907);
    CU_ASSERT_EQUAL(gengo_to_ad_1("明治", 40), 1907);
}

void test_skk_gadgets_func_gengo_to_ad_1_keio(void) {
    /* not found */
    CU_ASSERT_EQUAL(gengo_to_ad_1("けいおう", 2), -1);
}

void testsuite_skk_gadgets_func_gengo_to_ad_1(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func gengo_to_ad_1", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 reiwa", test_skk_gadgets_func_gengo_to_ad_1_reiwa);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 heisei", test_skk_gadgets_func_gengo_to_ad_1_heisei);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 shouwa", test_skk_gadgets_func_gengo_to_ad_1_shouwa);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 taishou", test_skk_gadgets_func_gengo_to_ad_1_taishou);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 meiji", test_skk_gadgets_func_gengo_to_ad_1_meiji);
    CU_add_test(suite, "skk-gadgets-func gengo_to_ad_1 keio", test_skk_gadgets_func_gengo_to_ad_1_keio);
}

/*
    ad_to_gengo_1
*/

void test_skk_gadgets_func_ad_to_gengo_1_notmatch(void) {
    Lisp_Object result = ad_to_gengo_1(lisp_env, 1867, true, 0, 0);
    CU_ASSERT_EQUAL(GET_TYPE(result), Internal_Error);
}

void test_skk_gadgets_func_ad_to_gengo_1_meiji_beg(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1868, true, 10, 23);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"明治\" \"M\" 1)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1868, false, 10, 23);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"明治\" \"M\" \"元\")");

    Lisp_Object result3 = ad_to_gengo_1(lisp_env, 1868, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result3), Internal_Error);
    writer(result3, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"明治\" \"M\" 1)");
}

void test_skk_gadgets_func_ad_to_gengo_1_meiji_end(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1912, true, 7, 29);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"明治\" \"M\" 45)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1911, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"明治\" \"M\" 44)");
}

void test_skk_gadgets_func_ad_to_gengo_1_taisho_beg(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1912, true, 7, 30);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"大正\" \"T\" 1)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1912, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"大正\" \"T\" 1)");
}

void test_skk_gadgets_func_ad_to_gengo_1_taisho_end(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1926, true, 12, 24);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"大正\" \"T\" 15)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1925, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"大正\" \"T\" 14)");
}

void test_skk_gadgets_func_ad_to_gengo_1_showa_beg(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1926, true, 12, 25);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"昭和\" \"S\" 1)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1926, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"昭和\" \"S\" 1)");
}

void test_skk_gadgets_func_ad_to_gengo_1_showa_end(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1989, true, 1, 7);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"昭和\" \"S\" 64)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1988, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"昭和\" \"S\" 63)");
}

void test_skk_gadgets_func_ad_to_gengo_1_heisei_beg(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 1989, true, 1, 8);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"平成\" \"H\" 1)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 1989, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"平成\" \"H\" 1)");
}

void test_skk_gadgets_func_ad_to_gengo_1_heisei_end(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 2019, true, 4, 30);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"平成\" \"H\" 31)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 2018, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"平成\" \"H\" 30)");
}

void test_skk_gadgets_func_ad_to_gengo_1_reiwa_beg(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object result1 = ad_to_gengo_1(lisp_env, 2019, true, 5, 1);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result1), Internal_Error);
    writer(result1, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"令和\" \"R\" 1)");

    Lisp_Object result2 = ad_to_gengo_1(lisp_env, 2019, true, 0, 0);
    CU_ASSERT_NOT_EQUAL(GET_TYPE(result2), Internal_Error);
    writer(result2, tmp_buf);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(\"令和\" \"R\" 1)");
}

void testsuite_skk_gadgets_func_ad_to_gengo_1(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func ad_to_gengo_1", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 nomatch", test_skk_gadgets_func_ad_to_gengo_1_notmatch);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 meiji begin", test_skk_gadgets_func_ad_to_gengo_1_meiji_beg);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 meiji end", test_skk_gadgets_func_ad_to_gengo_1_meiji_end);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 taisho begin", test_skk_gadgets_func_ad_to_gengo_1_taisho_beg);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 taisho end", test_skk_gadgets_func_ad_to_gengo_1_taisho_end);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 showa begin", test_skk_gadgets_func_ad_to_gengo_1_showa_beg);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 showa end", test_skk_gadgets_func_ad_to_gengo_1_showa_end);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 heisei begin", test_skk_gadgets_func_ad_to_gengo_1_heisei_beg);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 heisei end", test_skk_gadgets_func_ad_to_gengo_1_heisei_end);
    CU_add_test(suite, "skk-gadgets-func ad_to_gengo_1 reiwa begin", test_skk_gadgets_func_ad_to_gengo_1_reiwa_beg);
}

/*
    Function: skk-ad-to-gengo
 */

void test_skk_gadgets_func_skk_ad_to_gengo_1arg(void) {
    char *num_list[] = { "2024", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result1 = eval_expr(reader("(skk-ad-to-gengo 0)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "令和6");

    Lisp_Object result2 = eval_expr(reader("(skk-ad-to-gengo 1)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "R6");
}

void test_skk_gadgets_func_skk_ad_to_gengo_2args(void) {
    char *num_list[] = { "2024", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result1 = eval_expr(reader("(skk-ad-to-gengo 0 nil)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "令和6");

    Lisp_Object result2 = eval_expr(reader("(skk-ad-to-gengo 0 \":\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "令和:6");
}

void test_skk_gadgets_func_skk_ad_to_gengo_3args(void) {
    char *num_list[] = { "2024", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result1 = eval_expr(reader("(skk-ad-to-gengo 0 nil nil)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "令和6");

    Lisp_Object result2 = eval_expr(reader("(skk-ad-to-gengo 0 nil \"年\")", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "令和6年");
}

void test_skk_gadgets_func_skk_ad_to_gengo_4args(void) {
    char *num_list[] = { "2019", NULL };
    register_skk_num_list(lisp_env, num_list);

    Lisp_Object result1 = eval_expr(reader("(skk-ad-to-gengo 0 nil \"年\" nil)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result1), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result1), "令和元年");

    Lisp_Object result2 = eval_expr(reader("(skk-ad-to-gengo 0 nil \"年\" 1)", lisp_env), lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(result2), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(result2), "令和1年");
}

void testsuite_skk_gadgets_func_skk_ad_to_gengo(void) {
    CU_pSuite suite = CU_add_suite("skk-gadgets-func skk-ad-to-gengo", init_for_func_skk_gadgets_test, end_for_func_skk_gadgets_test);
    CU_add_test(suite, "skk-gadgets-func skk-ad-to-gengo with 1 arg", test_skk_gadgets_func_skk_ad_to_gengo_1arg);
    CU_add_test(suite, "skk-gadgets-func skk-ad-to-gengo with 2 args", test_skk_gadgets_func_skk_ad_to_gengo_2args);
    CU_add_test(suite, "skk-gadgets-func skk-ad-to-gengo with 3 args", test_skk_gadgets_func_skk_ad_to_gengo_3args);
    CU_add_test(suite, "skk-gadgets-func skk-ad-to-gengo with 4 args", test_skk_gadgets_func_skk_ad_to_gengo_4args);
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
    testsuite_skk_gadgets_func_skk_times();
    testsuite_skk_gadgets_func_skk_gadget_units_conversion();
    testsuite_skk_gadgets_func_gengo_to_ad_1();
    testsuite_skk_gadgets_func_ad_to_gengo_1();
    testsuite_skk_gadgets_func_skk_ad_to_gengo();
    testsuite_skk_gadgets_func_skk_num_list();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
