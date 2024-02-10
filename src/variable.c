#include <stdlib.h>
#include "lispobject.h"
#include "variable.h"

#define VARIABLE_STACK_SIZE 256

int set_variable(variable_pool_t *vp, char *sym, Lisp_Object val) {
    if (vp->variable_stack_used < VARIABLE_STACK_SIZE) {
        vp->variable_stack[vp->variable_stack_used++] = (variable_binding){ .symbol = sym, .value = val };
        return 0;
    } else {
        return -1;
    }
}

/* changing value of variable is not implemented */

Lisp_Object get_variable(variable_pool_t *vp, char *sym) {
    for (int i = vp->variable_stack_used - 1; i >= 0; i--) {
        if (vp->variable_stack[i] .symbol == sym) {
            return vp->variable_stack[i].value;
        }
    }
    // not found
    return (Lisp_Object){ .type = Internal_Error, .val.err = Variable_Error };
}

int save_variable_status(variable_pool_t *vp) {
    return vp->variable_stack_used;
}

void restore_variable_status(variable_pool_t *vp, int var_status) {
    vp->variable_stack_used = var_status;
}

void end_variables(variable_pool_t *variable_pool) {
    if (variable_pool->variable_stack != NULL) {
        free(variable_pool->variable_stack);
    }
    free(variable_pool);
}

variable_pool_t *init_variables(symbol_pool_t *symbol_pool) {
    variable_pool_t *variable_pool = malloc(sizeof(variable_pool_t));
    if (variable_pool == NULL) {
        return NULL;
    }

    variable_binding *variable_stack = malloc(sizeof(variable_binding) * VARIABLE_STACK_SIZE);
    if (variable_stack == NULL) {
        free(variable_pool);
        return NULL;
    }
    variable_pool->variable_stack = variable_stack;
    variable_pool->variable_stack_used = 0;

    variable_pool->symbol_pool = symbol_pool;

    return variable_pool;
}
