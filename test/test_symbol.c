#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/symbol.c"

/*
  Main
 */

int main(void) {
    CU_initialize_registry();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
