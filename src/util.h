/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>

typedef enum { R_NOTNUM, R_INT, R_FLOAT } check_num_t;

extern check_num_t check_num_str(const char *, bool);

extern int skk_num_type1_kanji(const char *, char *, size_t);
extern int skk_num_type3_kanji(const char *, char *, size_t);
extern int skk_num_exp(const char *, size_t, char *, size_t);

#endif /* _UTIL_H */
