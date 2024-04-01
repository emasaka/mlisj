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
    testsuite_skk_gadgets_func_skk_num_list();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
