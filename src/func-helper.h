#ifndef _FUNC_HELPER_H
#define _FUNC_HELPER_H

/* syntax sugar macros */
#define CHECK_CONDITION(c) if (!(c)) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_TYPE(x, tp) if (GET_TYPE(x) != tp) { return LISP_ERROR(Evaluation_Error); }
#define CHECK_ALLOC(p) if ((p) == NULL) { return LISP_ERROR(Memory_Error); }

#define ADD_FUNC_OR_RETURN(fp, s, f) if(add_func_from_cstr(fp, s, f, false) != 0) { return -1; }
#define SET_VARIABVLE_OR_RETURN(vp, var, val) if(set_variable_from_cstr(vp, var, val, false) != 0) { return -1; }

#endif /* _FUNC_HELPER_H */
