/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdint.h>

#ifndef _LISPOBJECT_H
#define _LISPOBJECT_H

/* 1: double as immediate value, 0: double as reference */
#define DOUBLE_IMMEDIATE 0

/* 0: default, 1: 64bit Lisp_Object in AMD64 architecture */
#define AMD64_TAGGEDPTR 0

enum Lisp_Type {
   Lisp_Symbol,
   Lisp_Int,
   Lisp_Float,
   Lisp_String,
   Lisp_CList,                  /* cdr-coded list */
   Lisp_Nil,
   Dynamic_Val,                 /* value of dynamic variable */

   Internal_Error               /* error code as Lisp_Object */

   /* not implemented: cons-list, vector */
};

enum Internal_Error_Type {
    Memory_Error,
    Reader_Error,
    Variable_Error,
    Evaluation_Error
};

#if AMD64_TAGGEDPTR
/* 64bit LispObject in AMD64 architecture */

#define DOUBLE_IMMEDIATE 0

typedef struct _Lisp_Object {
    enum Lisp_Type type: 4;
    unsigned long body:60;
} Lisp_Object;

/* accessors */
#define LISP_SYMBOL(x) ((Lisp_Object){ .type = Lisp_Symbol, .body = (uintptr_t)(x) })
#define LISP_INT(n) ((Lisp_Object){ .type = Lisp_Int, .body = (unsigned long)(long)(n) })
#define LISP_FLOAT(x) ((Lisp_Object){ .type = Lisp_Float, .body = (uintptr_t)(x) })
#define LISP_STRING(x) ((Lisp_Object){ .type = Lisp_String, .body = (uintptr_t)(x) })
#define LISP_CLIST(x) ((Lisp_Object){ .type = Lisp_CList, .body = (uintptr_t)(x) })
#define LISP_NIL ((Lisp_Object){ .type = Lisp_Nil, .body = 0 })
#define DYNAMIC_VAL(f) ((Lisp_Object){ .type = Dynamic_Val, .body = (uintptr_t)(f) })
#define LISP_ERROR(x) ((Lisp_Object){ .type = Internal_Error, .body = (unsigned long)(x) })

#define GET_TYPE(x) ((x).type)
#define GET_IVAL(x) ((int)(unsigned int)((x).body))
#define GET_FVAL(x) ((double *)(uintptr_t)((x).body))
#define GET_SVAL(x) ((char *)(uintptr_t)((x).body))
#define GET_AVAL(x) ((struct _NArray *)(uintptr_t)((x).body))
#define GET_FNVAL(x) ((Lisp_Object (*)(struct _lispenv *))(uintptr_t)((x).body))
#define GET_ERROR_TYPE(x) ((enum Internal_Error_Type)(uintptr_t)((x).body))

#else
/* default implementation */

/* prototype declarations of struct */
struct _Narray;
struct _lispenv;

/* naive implementation */
typedef struct _Lisp_Object {
    union {
        // struct _Lisp_Object *lval;    /* ununsed: cons list */
        int ival;               /* Lisp_Int, Lisp_Nil (0) */
#if DOUBLE_IMMEDIATE
        double fval;            /* Lisp_Float */
#else
        double *fval;           /* Lisp_Float */
#endif
        char *sval;             /* Lisp_String, Lisp_Symbol */
        struct _Lisp_Object (*fnval)(struct _lispenv *); /* Dynamic_Val */
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
#define DYNAMIC_VAL(f) ((Lisp_Object){ .type = Dynamic_Val, .val.fnval = (f) })
#define LISP_ERROR(x) ((Lisp_Object){ .type = Internal_Error, .val.err = x })

#define GET_TYPE(x) ((x).type)
#define GET_IVAL(x) ((x).val.ival)
#define GET_FVAL(x) ((x).val.fval)
#define GET_SVAL(x) ((x).val.sval)
#define GET_AVAL(x) ((x).val.aval)
#define GET_FNVAL(x) ((x).val.fnval)
#define GET_ERROR_TYPE(x) ((x).val.err)

#endif /* AMD64_TAGGEDPTR */

#if DOUBLE_IMMEDIATE
#define GET_FVAL_VAL(x) GET_FVAL(x)
#else
#define GET_FVAL_VAL(x) *(GET_FVAL(x))
#endif

#endif /* _LISPOBJECT_H */
