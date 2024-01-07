#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/symbol.c"

int init_for_symboltest(void) {
    init_mempool();
    init_symbol();
    return 0;
}

/*
  reserved symbols
 */

void test_symbol_quote(void) {
    char *q = symbol_table_lookup("quote");
    CU_ASSERT_PTR_NOT_NULL(q);
    CU_ASSERT_STRING_EQUAL(q, "quote");
}

void test_symbol_minus(void) {
    char *m = symbol_table_lookup("-");
    CU_ASSERT_PTR_NOT_NULL(m);
    CU_ASSERT_STRING_EQUAL(m, "-");
}

void testsuite_reserved_symbols(void) {
    CU_pSuite suite = CU_add_suite("reserved_symbols_test", init_for_symboltest, NULL);
    CU_add_test(suite, "symbol_quote", test_symbol_quote);
    CU_add_test(suite, "symbol_minus", test_symbol_minus);
}

/*
  adding symbols
 */

void test_add_symbol_copy(void) {
    char *str1 = "abc";
    char *sym1 = str2symbol(str1, true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_NOT_EQUAL(str1, sym1);
    CU_ASSERT_STRING_EQUAL(str1, sym1);

    char *str2 = "abc";
    char *sym2 = str2symbol(str2, true);
    CU_ASSERT_EQUAL(sym1, sym2);
}

void test_add_symbol_nocopy(void) {
    char *str1 = "def";
    char *sym1 = str2symbol(str1, false);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_EQUAL(str1, sym1);
}

void testsuite_adding_symbols(void) {
    CU_pSuite suite = CU_add_suite("adding_symbols_test", init_for_symboltest, NULL);
    CU_add_test(suite, "add_symbol_copy", test_add_symbol_copy);
    CU_add_test(suite, "add_symbol_nocopy", test_add_symbol_nocopy);
}

/*
  edge case
 */

void test_symbol_edge(void) {
    symbol_table_used = SYMBOL_TABLE_SIZE - 1;
    char *sym1 = new_symbol("ghi", true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    char *sym2 = new_symbol("jkh", true);
    CU_ASSERT_PTR_NULL(sym2);
}

void testsuite_symbols_edge(void) {
    CU_pSuite suite = CU_add_suite("adding_symbols_edge", init_for_symboltest, NULL);
    CU_add_test(suite, "add_symbol_edge", test_symbol_edge);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_reserved_symbols();
    testsuite_adding_symbols();
    testsuite_symbols_edge();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
