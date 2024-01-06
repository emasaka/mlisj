#ifndef _LISPOBJECT_H
#define _LISPOBJECT_H

enum Lisp_Type {
   Lisp_Symbol,
   Lisp_Int,
   Lisp_Float,
   Lisp_String,
   Lisp_CList,                  /* cdr-coded list */
   Lisp_Nil,

   Internal_Error               /* error code as Lisp_Object */

   /* not implemented: cons-list, vector */
};

enum Internal_Error_Type {
    Memory_Error,
    Reader_Error,
    Variable_Error
};

struct _Narray;                 /* prototype declaration */

/* naive implementation */
typedef struct _Lisp_Object {
    union {
        struct _Lisp_Object *lval;
        int ival;               /* Lisp_Int, Lisp_Nil (0) */
        double *fval;           /* Lisp_Float */
        char *sval;             /* Lisp_String, Lisp_Symbol */
        struct _NArray *aval;   /* Lisp_CList */
        enum Internal_Error_Type err; /* Internal_Error */

        /* TODO: C functions */

    } val;
    enum Lisp_Type type;
} Lisp_Object;

#endif /* _LISPOBJECT_H */
