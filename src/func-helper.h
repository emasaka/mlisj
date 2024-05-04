#ifndef _FUNC_HELPER_H
#define _FUNC_HELPER_H

#include <limits.h>

/* syntax sugar macros */
#define CHECK_CONDITION(c) if (!(c)) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_TYPE(x, tp) if (GET_TYPE(x) != tp) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_ALLOC(p) if ((p) == NULL) { return LISP_ERROR(Memory_Error); }

#define ADD_FUNC_OR_RETURN(fp, s, f) if(add_func_from_cstr(fp, s, f, false) != 0) { return -1; }
#define SET_VARIABVLE_OR_RETURN(vp, var, val) if(set_variable_from_cstr(vp, var, val, false) != 0) { return -1; }

#define OPTIONAL_ARG(ary, nth) (((ary)->size <= (nth)) ? LISP_NIL : (ary)->data[(nth)])

/* log_10(2) (roundup) */
#define LOG_10_2 0.302
/* max string length of unsigned int */
#define UINT_STRLEN ((size_t)(sizeof(int) * 8 * LOG_10_2 + 1))
/* max string length of int */
#define INT_STRLEN ((size_t)(((sizeof(int) * 8 - 1) * LOG_10_2 + 1 + 1)))

/* buffer size for strftime() ("Thu May  2 14:01:18 2024") */
#define STRFTIME_BUFFSIZE (4 + 4 + 3 + 8 + 1 + INT_STRLEN + 1)

#endif /* _FUNC_HELPER_H */
