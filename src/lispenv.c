#include <stdlib.h>
#include "lispenv.h"

void end_lispenv(lispenv_t *env) {
    if (env->func_pool != NULL) { end_func_table(env->func_pool); }
    if (env->variable_pool != NULL) { end_variables(env->variable_pool); }
    if (env->symbol_pool != NULL) { end_symbol(env->symbol_pool); }
    if (env->mempool != NULL) { end_mempool(env->mempool); }
    free(env);
}

lispenv_t *init_lispenv(void) {
    lispenv_t *env = malloc(sizeof(lispenv_t));
    if (env == NULL) {
        return NULL;
    }

    env->mempool = NULL;
    env->symbol_pool = NULL;
    env->variable_pool = NULL;
    env->func_pool = NULL;

    mempool_t *mempool = init_mempool();
    if (mempool == NULL) {
        end_lispenv(env);
        return NULL;
    }
    env->mempool = mempool;

    symbol_pool_t *symbol_pool = init_symbol(mempool);
    if (symbol_pool == NULL) {
        end_lispenv(env);
        return NULL;
    }
    env->symbol_pool = symbol_pool;

    env->Symbol_minus = str2symbol(symbol_pool, "-", false);
    env->Symbol_quote = str2symbol(symbol_pool, "quote", false);
    env->Symbol_lambda = str2symbol(symbol_pool, "lambda", false);

    variable_pool_t *variable_pool = init_variables(symbol_pool);
    if (variable_pool == NULL) {
        end_lispenv(env);
        return NULL;
    }
    env->variable_pool = variable_pool;

    func_pool_t *func_pool = init_func_table(symbol_pool);
    if (func_pool == NULL) {
        end_lispenv(env);
        return NULL;
    }
    env->func_pool = func_pool;

    return env;
}
