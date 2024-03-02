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
    Variable_Error,
    Evaluation_Error
};

struct _Narray;                 /* prototype declaration */

/* naive implementation */
typedef struct _Lisp_Object {
    union {
        // struct _Lisp_Object *lval;    /* ununsed: cons list */
        int ival;               /* Lisp_Int, Lisp_Nil (0) */
        double *fval;           /* Lisp_Float */
        char *sval;             /* Lisp_String, Lisp_Symbol */
        struct _NArray *aval;   /* Lisp_CList */
        enum Internal_Error_Type err; /* Internal_Error */
    } val;
    enum Lisp_Type type;
} Lisp_Object;

/* accessors */
#define LISP_SYMBOL(x) ((Lisp_Object){ .type = Lisp_Symbol, .val.sval = (x) })
#define LISP_INT(n) ((Lisp_Object){ .type = Lisp_Int, .val.ival = (n) })
#define LISP_FLOAT(x) ((Lisp_Object){ .type = Lisp_Float, .val.fval = (x) })
#define LISP_STRING(x) ((Lisp_Object){ .type = Lisp_String, .val.sval = (x) })
#define LISP_CLIST(x) ((Lisp_Object){ .type = Lisp_CList, .val.aval = (x) })
#define LISP_NIL ((Lisp_Object){ .type = Lisp_Nil, .val.ival = 0 })
#define LISP_ERROR(x) ((Lisp_Object){ .type = Internal_Error, .val.err = x })

#define GET_TYPE(x) ((x).type)
#define GET_IVAL(x) ((x).val.ival)
#define GET_FVAL(x) ((x).val.fval)
#define GET_SVAL(x) ((x).val.sval)
#define GET_AVAL(x) ((x).val.aval)
#define GET_ERROR_TYPE(x) ((x).val.err)

#endif /* _LISPOBJECT_H */
