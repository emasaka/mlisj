#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "util.h"

check_num_t check_num_str(char *str, bool num_only_p) {
    /* XXX: exponential notation of float is not supported */

    enum { START, DIGITS, DOT, DIGITS_AND_DOT } state = START;

    char *p = str;
    if (*p == '\0') {
        return R_NOTNUM;
    } else if (*p == '-' || *p == '+') {
        p++;
    }

    while (true) {
        switch (state){
        case START:
            if (isdigit(*p)) {
                state = DIGITS;
            } else if (*p == '.') {
                state = DOT;
            } else {
                return R_NOTNUM;
            }
            break;
        case DIGITS:
            if (isdigit(*p)) {
                state = DIGITS;
            } else if (*p == '.') {
                state = DIGITS_AND_DOT;
            } else {
                if (num_only_p == true &&  *p != '\0') {
                    return R_NOTNUM;
                } else {
                    return R_INT;
                }
            }
            break;
        case DOT:
            if (isdigit(*p)) {
                state = DIGITS_AND_DOT;
            } else {
                return R_NOTNUM;
            }
            break;
        case DIGITS_AND_DOT:
            if (isdigit(*p)) {
                state = DIGITS_AND_DOT;
            } else {
                if (num_only_p == true &&  *p != '\0') {
                    return R_NOTNUM;
                } else {
                    return R_FLOAT;
                }
            }
            break;
        }
        p++;
    }
}
