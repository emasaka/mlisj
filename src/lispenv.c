/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdlib.h>
#include <time.h>
#include "lispenv.h"

void get_current_tm(struct tm *tm_ptr) {
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, tm_ptr);
}

void end_lispenv(lispenv_t *env) {
    if (env->func_pool != NULL) { end_func_table(env->func_pool); }
    if (env->variable_pool != NULL) { end_variables(env->variable_pool); }
    if (env->symbol_pool != NULL) { end_symbol(env->symbol_pool); }
    if (env->mempool != NULL) { end_mempool(env->mempool); }
    free(env);
}

lispenv_t *init_lispenv(void) {
#define END_AND_RETURN_NULL(e) { end_lispenv(e); return NULL; }

    lispenv_t *env = malloc(sizeof(lispenv_t));
    if (env == NULL) { return NULL; }

    env->mempool = NULL;
    env->symbol_pool = NULL;
    env->variable_pool = NULL;
    env->func_pool = NULL;

    mempool_t *mempool = init_mempool();
    if (mempool == NULL) { END_AND_RETURN_NULL(env); }
    env->mempool = mempool;

    symbol_pool_t *symbol_pool = init_symbol(mempool);
    if (symbol_pool == NULL) { END_AND_RETURN_NULL(env); }

    env->symbol_pool = symbol_pool;

    env->Symbol_quote = str2symbol(symbol_pool, "quote", false);
    env->Symbol_lambda = str2symbol(symbol_pool, "lambda", false);

    variable_pool_t *variable_pool = init_variables(symbol_pool);
    if (variable_pool == NULL) { END_AND_RETURN_NULL(env); }
    env->variable_pool = variable_pool;

    func_pool_t *func_pool = init_func_table(symbol_pool);
    if (func_pool == NULL) { END_AND_RETURN_NULL(env); }
    env->func_pool = func_pool;

    env->skk_num_list = NULL;
    env->skk_henkan_key = NULL;

    /* pluggable for test */
    env->current_time_func = get_current_tm;

    return env;
#undef END_AND_RETURN_NULL
}

void register_skk_num_list(lispenv_t *env, char **skk_num_list) {
    env->skk_num_list = skk_num_list;
}

void register_skk_henkan_key(lispenv_t *env, const char *skk_henkan_key) {
    env->skk_henkan_key = skk_henkan_key;
}
