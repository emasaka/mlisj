#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>

typedef enum { R_NOTNUM, R_INT, R_FLOAT } check_num_t;

extern check_num_t check_num_str(char *, bool);

extern int skk_num_type1_kanji(const char *, char *, size_t);
extern int skk_num_type3_kanji(char *, char *, size_t);
extern int skk_num_exp(const char *, size_t, char *, size_t);

#endif /* _UTIL_H */
