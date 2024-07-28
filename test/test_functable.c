/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/mempool.h"
#include "../src/symbol.h"
#include "../src/functable.h"

static func_pool_t *func_pool;
static symbol_pool_t *symbol_pool;
static mempool_t *mempool;

int init_for_functabletest(void) {
    mempool = init_mempool();
    if (mempool == NULL) {
        return -1;
    }
    symbol_pool = init_symbol(mempool);
    if (symbol_pool == NULL) {
        end_mempool(mempool);
        return -1;
    }
    func_pool = init_func_table(symbol_pool);
    if (func_pool == NULL) {
        end_symbol(symbol_pool);
        end_mempool(mempool);
        return -1;
    } else {
        return 0;
    }
}

int end_for_functabletest(void) {
    end_func_table(func_pool);
    end_symbol(symbol_pool);
    end_mempool(mempool);
    return 0;
}

Lisp_Object dummy_func1(NArray *args, __attribute__((unused)) struct _lispenv *env) {
    return LISP_INT(GET_IVAL(args->data[0]) + 1);
}

Lisp_Object dummy_func2(NArray *args, __attribute__((unused)) struct _lispenv *env) {
    return LISP_INT(GET_IVAL(args->data[0]) + 2);
}

void test_functable_found(void) {
    char *str1 = "foo";
    char *str2 = "bar";
    CU_ASSERT(add_func_from_cstr(func_pool, str1, dummy_func1, true) == 0);
    CU_ASSERT(add_func_from_cstr(func_pool, str2, dummy_func2, true) == 0);

    char *sym1 = str2symbol(func_pool->symbol_pool, str1, true);
    char *sym2 = str2symbol(func_pool->symbol_pool, str2, true);
    CU_ASSERT_PTR_NOT_NULL(sym1);
    CU_ASSERT_PTR_NOT_NULL(sym2);

    NArray *args = new_narray(mempool, 1);
    CU_ASSERT_PTR_NOT_NULL(args);
    args->data[0] = LISP_INT(3);

    cfunc_t func1 = get_func(func_pool, sym1);
    CU_ASSERT_PTR_NOT_NULL(func1);
    CU_ASSERT_EQUAL(GET_IVAL(func1(args, NULL)), 4);

    cfunc_t func2 = get_func(func_pool, sym2);
    CU_ASSERT_PTR_NOT_NULL(func2);
    CU_ASSERT_EQUAL(GET_IVAL(func2(args, NULL)), 5);
}

void test_functable_notfound(void) {
    char *sym = str2symbol(func_pool->symbol_pool, "baz", true);
    CU_ASSERT_PTR_NOT_NULL(sym);
    CU_ASSERT_PTR_NULL(get_func(func_pool, sym));
}

void testsuite_functable_set_get(void) {
    CU_pSuite suite = CU_add_suite("functable_set_get", init_for_functabletest, end_for_functabletest);
    CU_add_test(suite, "functable_found", test_functable_found);
    CU_add_test(suite, "functable_notfound", test_functable_notfound);
}

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    testsuite_functable_set_get();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();

    return ret;
}
