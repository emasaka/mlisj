/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/reader.h"
#include "helper/writer.h"

#define TMP_BUFFSIZE 128

static lispenv_t *lisp_env;

int init_for_readertest(void) {
    lisp_env =init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    } else {
        return 0;

    }
}

int end_for_readertest(void) {
    end_lispenv(lisp_env);
    return 0;
}

/*
  numbers
 */

void test_reader_positive_int(void) {
    Lisp_Object obj = reader("31", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), 31);
}

void test_reader_positive_int_with_prefix(void) {
    Lisp_Object obj = reader("+31", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), 31);
}

void test_reader_negative_int(void) {
    Lisp_Object obj = reader("-31", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), -31);
}

void test_reader_char(void) {
    Lisp_Object obj = reader("?a", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), 'a');
}

void test_reader_floatnum(void) {
    Lisp_Object obj = reader("3.0", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_Float);
    CU_ASSERT_DOUBLE_EQUAL(GET_FVAL_VAL(obj), 3.0, 1e-10);
}

void testsuite_reader_number(void) {
    CU_pSuite suite = CU_add_suite("reader_number", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_positive_int", test_reader_positive_int);
    CU_add_test(suite, "reader_positive_int_with_prefix", test_reader_positive_int_with_prefix);
    CU_add_test(suite, "reader_negative_int", test_reader_negative_int);
    CU_add_test(suite, "reader_char", test_reader_char);
    CU_add_test(suite, "reader_floatnum", test_reader_floatnum);
}

/*
  symbol
 */

void test_reader_symbol(void) {
    char tmp_buf[TMP_BUFFSIZE];

    Lisp_Object obj1 = reader("foo", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj1), Lisp_Symbol);
    CU_ASSERT(writer(obj1, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "foo");

    /* '-' is a symbol, and '-1' is a number */
    Lisp_Object obj2 = reader("-", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj2), Lisp_Symbol);
    CU_ASSERT(writer(obj2, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "-");
}

void testsuite_reader_symbol(void) {
    CU_pSuite suite = CU_add_suite("reader_symbol", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_symbol", test_reader_symbol);
}

/*
  '#' + integer
 */

void test_reader_hashnum_hex(void) {
    Lisp_Object obj = reader("#x21", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), 0x21);
}

void test_reader_hashnum_oct(void) {
    Lisp_Object obj = reader("#o21", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Int);
    CU_ASSERT_EQUAL(GET_IVAL(obj), 021);
}

void test_reader_hashnum_error1(void) {
    Lisp_Object obj = reader("#W", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
    CU_ASSERT_EQUAL(GET_ERROR_TYPE(obj), Reader_Error);
}

void test_reader_hashnum_error2(void) {
    Lisp_Object obj = reader("#x11W", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
    CU_ASSERT_EQUAL(GET_ERROR_TYPE(obj), Reader_Error);
}

void testsuite_reader_hashnum(void) {
    CU_pSuite suite = CU_add_suite("reader_hashnum", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_hashnum_hex", test_reader_hashnum_hex);
    CU_add_test(suite, "reader_hashnum_oct", test_reader_hashnum_oct);
    CU_add_test(suite, "reader_hashnum_error1", test_reader_hashnum_error1);
    CU_add_test(suite, "reader_hashnum_error2", test_reader_hashnum_error2);
}

/*
  nil
 */

void test_reader_nil(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("nil", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Lisp_Nil);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "nil");
}

void testsuite_reader_nil(void) {
    CU_pSuite suite = CU_add_suite("reader_nil", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_nil", test_reader_nil);
}

/*
  string
 */

void test_reader_string_visible(void) {
    Lisp_Object obj = reader("\"Hello, World!\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "Hello, World!");
}

void test_reader_string_escaped_octet1(void) {
    Lisp_Object obj = reader("\"I\\057O\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "I/O");
}

void test_reader_string_escaped_octet2(void) {
    Lisp_Object obj = reader("\"\\033\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "\033");
}

void test_reader_string_escaped_octet3(void) {
    Lisp_Object obj = reader("\"\\06100\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "100");
}

void test_reader_string_escaped_octet4(void) {
    Lisp_Object obj = reader("\"\\100\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "@");
}

void test_reader_string_escaped_hex1(void) {
    Lisp_Object obj = reader("\"\\x5a\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "Z");
}

void test_reader_string_escaped_hex2(void) {
    Lisp_Object obj = reader("\"\\x5A\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "Z");
}

void test_reader_string_newline(void) {
    Lisp_Object obj = reader("\"a\\nb\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "a\nb");
}

void test_reader_string_escaped_visible(void) {
    Lisp_Object obj = reader("\"\\W\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "W");
}

void test_reader_string_escaped_ignore1(void) {
    Lisp_Object obj = reader("\"\\61\\ 9\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "19");
}

void test_reader_string_escaped_ignore2(void) {
    Lisp_Object obj = reader("\"a\\\nb\"", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_String);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "ab");
}

void test_reader_string_broken(void) {
    Lisp_Object obj = reader("\"", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
}

void testsuite_reader_string(void) {
    CU_pSuite suite = CU_add_suite("reader_string", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_string_visible", test_reader_string_visible);
    CU_add_test(suite, "reader_string_escaped_octet1", test_reader_string_escaped_octet1);
    CU_add_test(suite, "reader_string_escaped_octet2", test_reader_string_escaped_octet2);
    CU_add_test(suite, "reader_string_escaped_octet3", test_reader_string_escaped_octet3);
    CU_add_test(suite, "reader_string_escaped_octet4", test_reader_string_escaped_octet4);
    CU_add_test(suite, "reader_string_escaped_hex1", test_reader_string_escaped_hex1);
    CU_add_test(suite, "reader_string_escaped_hex2", test_reader_string_escaped_hex2);
    CU_add_test(suite, "reader_string_newline", test_reader_string_newline);
    CU_add_test(suite, "reader_string_escaped_visible", test_reader_string_escaped_visible);
    CU_add_test(suite, "reader_string_escaped_ignore1", test_reader_string_escaped_ignore1);
    CU_add_test(suite, "reader_string_escaped_ignore2", test_reader_string_escaped_ignore2);
    CU_add_test(suite, "reader_string_broken", test_reader_string_broken);
}

/*
  list
 */

void test_reader_list_empty(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("()", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "()");
}

void test_reader_list_one(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(foo)", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(foo)");
}

void test_reader_list_two(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(foo bar)", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(foo bar)");
}

void test_reader_list_quoted(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("'foo", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(quote foo)");
}

void test_reader_list_nested1(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("((foo))", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "((foo))");
}

void test_reader_list_nested2(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(lambda (a b) (concat a b))", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(lambda (a b) (concat a b))");
}

void test_reader_list_broken(void) {
    Lisp_Object obj = reader("(", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
}

void test_reader_list_nested_toodeep(void) {
    char tmp_buf[(READER_MAX_NESTING_LEVEL + 1) * 2 + 1];
    char *p = tmp_buf;
    for (size_t i = 0; i < READER_MAX_NESTING_LEVEL + 1; i++){
        *p++ = '(';
    }
    for (size_t i = 0; i < READER_MAX_NESTING_LEVEL + 1; i++){
        *p++ = ')';
    }
    *p = '\0';
    Lisp_Object obj = reader(tmp_buf, lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
}

void testsuite_reader_list(void) {
    CU_pSuite suite = CU_add_suite("reader_list", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_list_empty", test_reader_list_empty);
    CU_add_test(suite, "reader_list_one", test_reader_list_one);
    CU_add_test(suite, "reader_list_two", test_reader_list_two);
    CU_add_test(suite, "reader_list_quoted", test_reader_list_quoted);
    CU_add_test(suite, "reader_list_nested1", test_reader_list_nested1);
    CU_add_test(suite, "reader_list_nested2", test_reader_list_nested2);
    CU_add_test(suite, "reader_list_broken", test_reader_list_broken);
    CU_add_test(suite, "reader_list_nested_toodeep", test_reader_list_nested_toodeep);
}

/*
  empty
 */

void test_reader_empty_null(void) {
    Lisp_Object obj = reader("", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
}

void test_reader_empty_only_spaces(void) {
    Lisp_Object obj = reader("  ", lisp_env);
    CU_ASSERT_EQUAL(GET_TYPE(obj), Internal_Error);
}

void testsuite_reader_empty(void) {
    CU_pSuite suite = CU_add_suite("reader_empty", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_empty_null", test_reader_empty_null);
    CU_add_test(suite, "reader_empty_only_spaces", test_reader_empty_only_spaces);
}

/*
  comment
 */

void test_reader_comment_skip(void) {
    Lisp_Object obj = reader("; comment\nfoo", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_Symbol);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "foo");
}

void test_reader_comment_symbol(void) {
    Lisp_Object obj = reader("foo;bar", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_Symbol);
    CU_ASSERT_STRING_EQUAL(GET_SVAL(obj), "foo");
}

void test_reader_comment_in_list(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(foo ;bar\n baz)", lisp_env);
    CU_ASSERT_EQUAL_FATAL(GET_TYPE(obj), Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(foo baz)");
}

void testsuite_reader_comment(void) {
    CU_pSuite suite = CU_add_suite("reader_comment", init_for_readertest, end_for_readertest);
    CU_add_test(suite, "reader_comment_skip", test_reader_comment_skip);
    CU_add_test(suite, "reader_comment_symbol", test_reader_comment_symbol);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_reader_number();
    testsuite_reader_symbol();
    testsuite_reader_hashnum();
    testsuite_reader_nil();
    testsuite_reader_string();
    testsuite_reader_list();
    testsuite_reader_empty();
    testsuite_reader_comment();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
