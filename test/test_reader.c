#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/reader.c"

/* from writer.c */
extern int writer(Lisp_Object, char *);

#define TMP_BUFFSIZE 128

int init_for_readertest(void) {
    init_mempool();
    init_symbol();
    return 0;
}

/*
  numbers
 */

void test_reader_positive_int(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("31");
    CU_ASSERT_EQUAL(obj.type, Lisp_Int);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "31");
}

void test_reader_negative_int(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("-31");
    CU_ASSERT_EQUAL(obj.type, Lisp_Int);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "-31");
}

void test_reader_char(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("?a");
    CU_ASSERT_EQUAL(obj.type, Lisp_Int);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "97");
}

void testsuite_reader_number(void) {
    CU_pSuite suite = CU_add_suite("reader_number", init_for_readertest, NULL);
    CU_add_test(suite, "reader_positive_int", test_reader_positive_int);
    CU_add_test(suite, "reader_negative_int", test_reader_negative_int);
    CU_add_test(suite, "reader_char", test_reader_char);
}

/*
  symbol
 */

void test_reader_symbol(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("foo");
    CU_ASSERT_EQUAL(obj.type, Lisp_Symbol);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "foo");
}

void testsuite_reader_symbol(void) {
    CU_pSuite suite = CU_add_suite("reader_symbol", init_for_readertest, NULL);
    CU_add_test(suite, "reader_symbol", test_reader_symbol);
}

/*
  nil
 */

void test_reader_nil(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("nil");
    CU_ASSERT_EQUAL(obj.type, Lisp_Nil);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "nil");
}

void testsuite_reader_nil(void) {
    CU_pSuite suite = CU_add_suite("reader_nil", init_for_readertest, NULL);
    CU_add_test(suite, "reader_nil", test_reader_nil);
}

/*
  string
 */

void test_reader_string_visible(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("\"Hello, World!\"");
    CU_ASSERT_EQUAL(obj.type, Lisp_String);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "\"Hello, World!\"");
}

void test_reader_string_escaped_octet1(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("\"I\\057O\"");
    CU_ASSERT_EQUAL(obj.type, Lisp_String);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "\"I/O\"");
}

void test_reader_string_escaped_octet2(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("\"\\033\"");
    CU_ASSERT_EQUAL(obj.type, Lisp_String);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "\"\\033\"");
}

void test_reader_string_escaped_visible(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("\"\\W\"");
    CU_ASSERT_EQUAL(obj.type, Lisp_String);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "\"W\"");
}

void testsuite_reader_string(void) {
    CU_pSuite suite = CU_add_suite("reader_string", init_for_readertest, NULL);
    CU_add_test(suite, "reader_string_visible", test_reader_string_visible);
    CU_add_test(suite, "reader_string_escaped_octet1", test_reader_string_escaped_octet1);
    CU_add_test(suite, "reader_string_escaped_octet2", test_reader_string_escaped_octet2);
    CU_add_test(suite, "reader_string_escaped_visible", test_reader_string_escaped_visible);
}

/*
  list
 */

void test_reader_list_empty(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("()");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "()");
}

void test_reader_list_one(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(foo)");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(foo)");
}

void test_reader_list_two(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(foo bar)");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(foo bar)");
}

void test_reader_list_quoted(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("'foo");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(quote foo)");
}

void test_reader_list_nested1(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("((foo))");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "((foo))");
}

void test_reader_list_nested2(void) {
    char tmp_buf[TMP_BUFFSIZE];
    Lisp_Object obj = reader("(lambda (a b) (concat a b))");
    CU_ASSERT_EQUAL(obj.type, Lisp_CList);
    CU_ASSERT(writer(obj, tmp_buf) == 0);
    CU_ASSERT_STRING_EQUAL(tmp_buf, "(lambda (a b) (concat a b))");
}

void testsuite_reader_list(void) {
    CU_pSuite suite = CU_add_suite("reader_list", init_for_readertest, NULL);
    CU_add_test(suite, "reader_list_empty", test_reader_list_empty);
    CU_add_test(suite, "reader_list_one", test_reader_list_one);
    CU_add_test(suite, "reader_list_two", test_reader_list_two);
    CU_add_test(suite, "reader_list_quoted", test_reader_list_quoted);
    CU_add_test(suite, "reader_list_nested1", test_reader_list_nested1);
    CU_add_test(suite, "reader_list_nested2", test_reader_list_nested2);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_reader_number();
    testsuite_reader_symbol();
    testsuite_reader_nil();
    testsuite_reader_string();
    testsuite_reader_list();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
