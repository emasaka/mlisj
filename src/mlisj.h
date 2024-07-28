/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#ifndef _MLISJ_H
#define _MLISJ_H

extern int mlisj_eval(const char *, char *, size_t, char **, const char *);

#define MLISJ_ERROR_EVALUATION 1
#define MLISJ_ERROR_RUNTIME 2

#endif /* _MLISJ_H */
