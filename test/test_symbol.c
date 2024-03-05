#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/symbol.h"

static symbol_pool_t *symbol_pool;
static mempool_t *mempool;

int init_for_symboltest(void) {
    mempool = init_mempool();
    if (mempool == NULL) {
        return -1;
    }
    symbol_pool = init_symbol(mempool);
    if (symbol_pool == NULL) {
        end_mempool(mempool);
        return -1;
    } else {
        return 0;
    }
}

int end_for_symboltest(void) {
    end_symbol(symbol_pool);
    end_mempool(mempool);
    return 0;
}

/*
  adding symbols
 */

void test_add_symbol_copy(void) {
    char *str1 = "abc";
    char *sym1 = str2symbol(symbol_pool, str1, true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_PTR_NOT_EQUAL(str1, sym1);
    CU_ASSERT_STRING_EQUAL(str1, sym1);

    char *str2 = "abc";
    char *sym2 = str2symbol(symbol_pool, str2, true);
    CU_ASSERT_PTR_EQUAL(sym1, sym2);
}

void test_add_symbol_nocopy(void) {
    char *str1 = "def";
    char *sym1 = str2symbol(symbol_pool, str1, false);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_PTR_EQUAL(str1, sym1);
}

void testsuite_adding_symbols(void) {
    CU_pSuite suite = CU_add_suite("adding_symbols_test", init_for_symboltest, end_for_symboltest);
    CU_add_test(suite, "add_symbol_copy", test_add_symbol_copy);
    CU_add_test(suite, "add_symbol_nocopy", test_add_symbol_nocopy);
}

/*
  edge case
 */

void test_symbol_edge(void) {
    symbol_pool->symbol_table_used = SYMBOL_TABLE_SIZE - 1;
    char *sym1 = new_symbol(symbol_pool, "ghi", true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    char *sym2 = new_symbol(symbol_pool, "jkh", true);
    CU_ASSERT_PTR_NULL(sym2);
}

void testsuite_symbols_edge(void) {
    CU_pSuite suite = CU_add_suite("adding_symbols_edge", init_for_symboltest, end_for_symboltest);
    CU_add_test(suite, "add_symbol_edge", test_symbol_edge);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_adding_symbols();
    testsuite_symbols_edge();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
