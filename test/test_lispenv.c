#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/lispenv.c"

static lispenv_t *lisp_env;

int init_for_lispenvtest(void) {
    lisp_env = init_lispenv();
    if (lisp_env == NULL) {
        return -1;
    } else {
        return 0;

    }
}

int end_for_lispenvtest(void) {
    end_lispenv(lisp_env);
    return 0;
}

/*
  reserved symbols
 */

void test_symbol_quote(void) {
    char *q = symbol_table_lookup(lisp_env->symbol_pool, "quote");
    CU_ASSERT_PTR_NOT_NULL(q);
    CU_ASSERT_STRING_EQUAL(q, "quote");
}

void test_symbol_minus(void) {
    char *m = symbol_table_lookup(lisp_env->symbol_pool, "-");
    CU_ASSERT_PTR_NOT_NULL(m);
    CU_ASSERT_STRING_EQUAL(m, "-");
}

void test_symbol_lambda(void) {
    char *m = symbol_table_lookup(lisp_env->symbol_pool, "lambda");
    CU_ASSERT_PTR_NOT_NULL(m);
    CU_ASSERT_STRING_EQUAL(m, "lambda");
}

void testsuite_reserved_symbols(void) {
    CU_pSuite suite = CU_add_suite("reserved_symbols_test", init_for_lispenvtest, end_for_lispenvtest);
    CU_add_test(suite, "symbol_quote", test_symbol_quote);
    CU_add_test(suite, "symbol_minus", test_symbol_minus);
    CU_add_test(suite, "symbol_lambda", test_symbol_lambda);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_reserved_symbols();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
