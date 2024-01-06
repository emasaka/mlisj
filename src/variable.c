#include "lispobject.h"

/* internal type */
typedef struct {
    char *symbol;
    Lisp_Object value;
} variable_binding;

#define VARIABLE_STACK_SIZE 256

static variable_binding variable_stack[VARIABLE_STACK_SIZE];

static int variable_stack_used = 0;

int set_variable(char *sym, Lisp_Object val) {
    if (variable_stack_used < VARIABLE_STACK_SIZE) {
        variable_stack[variable_stack_used++] = (variable_binding){ .symbol = sym, .value = val };
        return 0;
    } else {
        return -1;
    }
}

/* changing value of variable is not implemented */

Lisp_Object get_variable(char *sym) {
    for (int i = variable_stack_used - 1; i >= 0; i--) {
        if (variable_stack[i] .symbol == sym) {
            return variable_stack[i].value;
        }
    }
    // not found
    return (Lisp_Object){ .type = Internal_Error, .val.err = Variable_Error };
}

int save_variable_status(void) {
    return variable_stack_used;
}

void restore_variable_status(int var_status) {
    variable_stack_used = var_status;
}

int init_variables(void) {
    variable_stack_used = 0;
    // return the value of success, just in case
    return 0;
}
