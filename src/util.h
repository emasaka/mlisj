#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>

typedef enum { R_NOTNUM, R_INT, R_FLOAT } check_num_t;

extern check_num_t check_num_str(char *, bool);

#endif /* _UTIL_H */
