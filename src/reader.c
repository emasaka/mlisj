#include <ctype.h>
#include <string.h>
#include <stdlib.h>
# include "lispenv.h"

#define READER_BUFSIZE 512
#define READER_ARRAY_BUFSIZE 128

typedef struct {
    char *ptr;
    lispenv_t *env;
} reader_context;

static Lisp_Object reader_sexp(reader_context *); /* prototype declaration */

/* parse integer number, or return nil */
static Lisp_Object reader_check_intnum(char *str) {
    /* XXX: only dicimal integer is supported */
    int minus = 1;
    char *p = str;
    if (str[0] == '-') {
        if (str[1] == '\0') {
            return LISP_NIL;
        } else {
            minus = -1;
            p++;
        }
    }
    int n = 0;
    while (*p != '\0') {
        if (isdigit(*p) ) {
            n = n * 10 + (*p - '0');
            p++;
        } else {
            /* not a number */
            return LISP_NIL;
        }
    }
    return LISP_INT(n * minus);
}

/* parse float number, or return nil */
static Lisp_Object reader_check_floatnum(char *str, lispenv_t *env) {
    char *endptr;
    double fnum = strtod(str, &endptr);
    if (*endptr != '\0') {
        return LISP_NIL;
    }
    double *flt = cdouble2float(env->mempool, fnum);
    if (flt == NULL) {
        return LISP_ERROR(Memory_Error);
    }
    return LISP_FLOAT(flt);
}

static Lisp_Object reader_maybe_symbol(reader_context *c) {
    char buffer[READER_BUFSIZE];
    char *p = buffer;
    for (size_t i = 0; i < READER_BUFSIZE; i++) {
        char ch = c->ptr[0];
        /* XXX: escapes in symbol is not supported */
        if (isspace(ch) || (ch == '(') || (ch == ')') || (ch == '\"') || (ch == '\'') || (ch == '\0')) {
            *p = '\0';

            /* integer number? */
            Lisp_Object n = reader_check_intnum(buffer);
            if (GET_TYPE(n) != Lisp_Nil) {
                return n;
            }

            /* float number? */
            Lisp_Object fnum = reader_check_floatnum(buffer, c->env);
            if (GET_TYPE(fnum) == Lisp_Float) {
                return fnum;
            }

            /* nil? */
            if (strcmp(buffer, "nil") == 0) {
                return LISP_NIL;
            }

            /* returns a symbol */
            char *sym = str2symbol(c->env->symbol_pool, buffer, true);
            if (sym == NULL) {
                return LISP_ERROR(Memory_Error);
            } else {
                return LISP_SYMBOL(sym);
            }
        } else {
            *p++ = ch;
            c->ptr++;
        }
    }
    /* too long */
    return LISP_ERROR(Reader_Error);
}

static Lisp_Object reader_char(reader_context *c) {
    /* XXX: escapes in character is not supported */
    /* XXX: multibyte character is not supported */
    c->ptr++;
    if (*(c->ptr) == '\0') { return LISP_ERROR(Reader_Error); }
    return LISP_INT(*(c->ptr++));
}

static Lisp_Object reader_string(reader_context *c) {
    /* XXX: "\x<hex>" and "\u<code>" are not supported */
    /* XXX: escaping multibyte character is not supported (maybe no problem) */
    c->ptr++;
    char buffer[READER_BUFSIZE];
    size_t buffer_used = 0;
    char *p = buffer;
    while (c->ptr[0] != '\0') {
        switch (c->ptr[0]) {
        case '\\':
            c->ptr++;
            if (c->ptr[0] == '0') {
                /* "\<octet>" */
                c->ptr++;
                int n = 0;
                while ((c->ptr[0] >= '0') && (c->ptr[0] <= '7')) {
                    n = (n << 3) + (c->ptr[0] - '0');
                    c->ptr++;
                }
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                *p++ = (char)n;
            } else {
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                *p++ = *(c->ptr++);
            }
            break;
        case '\"':
            if (buffer_used == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
            *p = '\0';
            c->ptr++;
            char *newstr = copy_to_string_area(c->env->mempool, buffer);
            if (newstr == NULL ) { return LISP_ERROR(Memory_Error); }
            return LISP_STRING(newstr);
            break;
        default:
            if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
            *p++ = *(c->ptr++);
            break;
        }
    }
    /* double-quote is not closed */
    return LISP_ERROR(Reader_Error);
}

static Lisp_Object reader_list(reader_context *c) {
    Lisp_Object buffer[READER_ARRAY_BUFSIZE];
    size_t buffer_used = 0;
    c->ptr++;
    for (size_t i = 0; i < READER_ARRAY_BUFSIZE; i++) {
        while (isspace(c->ptr[0])) { c->ptr++; }
        if (c->ptr[0] == ')') {
            c->ptr++;
            NArray *ary = new_narray(c->env->mempool, buffer_used);
            if (ary == NULL ) { return LISP_ERROR(Memory_Error); }
            for (size_t j = 0; j < buffer_used; j++) {
                ary->data[j] = buffer[j];
            }
            ary->size = buffer_used;
            return LISP_CLIST(ary);
        }

        Lisp_Object v = reader_sexp(c);
        if (GET_TYPE(v) == Internal_Error ) { return v; }
        if (buffer_used == READER_ARRAY_BUFSIZE)  { return LISP_ERROR(Memory_Error); }
        buffer[buffer_used++] = v;
    }
    /* too long */
    return LISP_ERROR(Reader_Error);
}

static Lisp_Object reader_quoted(reader_context *c) {
    c->ptr++;
    Lisp_Object v = reader_sexp(c);
    if (GET_TYPE(v) == Internal_Error ) { return v; }
    NArray *ary = new_narray(c->env->mempool, 2);
    if (ary == NULL ) { return LISP_ERROR(Memory_Error); }
    ary->data[0] = LISP_SYMBOL(c->env->Symbol_quote);
    ary->data[1] = v;
    return LISP_CLIST(ary);
}

static Lisp_Object reader_sexp(reader_context *c) {
    while (isspace(c->ptr[0])) { c->ptr++; }
    if (c->ptr[0] == '\0') {
        return LISP_ERROR(Reader_Error);
    } else if (c->ptr[0] == '\'') {
        return reader_quoted(c);
    } else if (c->ptr[0] == '(') {
        return reader_list(c);
    } else if (c->ptr[0] == '\"') {
        return reader_string(c);
    } else if (c->ptr[0] == '?') {
        return reader_char(c);
    } else {
        return reader_maybe_symbol(c);
    }
}

Lisp_Object reader(char *str, lispenv_t *env) {
    reader_context c = (reader_context){ .ptr = str, .env = env };
    return reader_sexp(&c);
}
