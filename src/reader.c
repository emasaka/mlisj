/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "lispenv.h"
#include "util.h"
#include "reader.h"

#define READER_BUFSIZE 512
#define READER_ARRAY_BUFSIZE 128

typedef struct {
    const char *ptr;
    size_t nesting_level;
    lispenv_t *env;
} reader_context;

static Lisp_Object reader_sexp(reader_context *); /* prototype declaration */

/* helper macros */
#define is_oct_digit(c) (((c) >= '0') && ((c) <= '7'))
#define xdigit2i(c) (isdigit(c) ? ((c) - '0') : islower(c) ? ((c) - 'a' + 10) : ((c) - 'A' + 10))
#define is_start_token(c) (isspace(c) || ((c) == '(') || ((c) == ')') || ((c) == '\"') || ((c) == '\'') || ((c) == ';') || ((c) == '\0'))

/* helper function */
static void reader_skip_spaces_and_comment(reader_context *c) {
    while (true) {
        while (isspace(c->ptr[0])) { c->ptr++; }
        if (c->ptr[0] == ';') {
            while ((c->ptr[0] != '\n') && (c->ptr[0] != '\0')) { c->ptr++; }
        } else {
            return;
        }
    }
}

/* parse '#' + integer */
static Lisp_Object reader_hashnum(reader_context *c) {
    /* NOTE: '#b' and '#<radix>' are not supported */
    int n = 0;

    c->ptr++;
    if (c->ptr[0] == 'x') {
        c->ptr++;
        while (isxdigit(c->ptr[0])) {
            n = (n << 4) + xdigit2i(c->ptr[0]);
            c->ptr++;
        }
    } else if (c->ptr[0] == 'o') {
        c->ptr++;
        while (is_oct_digit(c->ptr[0])) {
            n = (n << 3) + (c->ptr[0] - '0');
            c->ptr++;
        }
    } else {
        return LISP_ERROR(Reader_Error);
    }

    if (is_start_token(c->ptr[0])) {
        return LISP_INT(n);
    } else {
        /* '#x11W' causes error */
        return LISP_ERROR(Reader_Error);
    }
}

/* parse integer number */
static Lisp_Object reader_get_intnum(const char *str) {
    errno = 0;
    int n = (int)strtol(str, NULL, 10);
    if (errno != 0) { return LISP_ERROR(Reader_Error); }
    return LISP_INT(n);
}

/* parse float number */
#if DOUBLE_IMMEDIATE
static Lisp_Object reader_get_floatnum(const char *str) {
    double fnum = strtod(str, NULL);
    return LISP_FLOAT(fnum);
}
#else
static Lisp_Object reader_get_floatnum(const char *str, lispenv_t *env) {
    double fnum = strtod(str, NULL);
    double *flt = cdouble2float(env->mempool, fnum);
    if (flt == NULL) {
        return LISP_ERROR(Memory_Error);
    }
    return LISP_FLOAT(flt);
}
#endif

static Lisp_Object reader_maybe_symbol(reader_context *c) {
    char buffer[READER_BUFSIZE];
    char *p = buffer;
    for (size_t i = 0; i < READER_BUFSIZE; i++) {
        char ch = c->ptr[0];
        /* NOTE: escapes in symbol is not supported */
        if (is_start_token(ch)) {
            *p = '\0';

            /* integer? float? other?*/
            check_num_t result = check_num_str(buffer, true);
            if (result == R_INT) {
                return reader_get_intnum(buffer);
            } else if (result == R_FLOAT) {
#if DOUBLE_IMMEDIATE
                return reader_get_floatnum(buffer);
#else
                return reader_get_floatnum(buffer, c->env);
#endif
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
    /* NOTE: escapes in character is not supported */
    /* NOTE: multibyte character is not supported */
    c->ptr++;
    if (*(c->ptr) == '\0') { return LISP_ERROR(Reader_Error); }
    return LISP_INT(*(c->ptr++));
}

static Lisp_Object reader_string(reader_context *c) {
    /* NOTE: "\u<code>" are not supported */
    /* NOTE: escaping multibyte character is not supported (maybe no problem) */
    c->ptr++;
    char buffer[READER_BUFSIZE];
    size_t buffer_used = 0;
    char *p = buffer;
    while (c->ptr[0] != '\0') {
        switch (c->ptr[0]) {
        case '\\':
            c->ptr++;
            if (is_oct_digit(c->ptr[0])) {
                /* "\<octet>" */
                int n = 0;
                size_t i = 0;
                do {
                    if (i++ >= 3) { break; }
                    n = (n << 3) + (c->ptr[0] - '0');
                    c->ptr++;
                } while (is_oct_digit(c->ptr[0]));
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                *p++ = (char)n;
            } else if (c->ptr[0] == 'x') {
                /* "\x<hex>" */
                c->ptr++;
                int n = 0;
                size_t i = 0;
                while (isxdigit(c->ptr[0])) {
                    if (i++ >= 2) { break; }
                    n = (n << 4) + xdigit2i(c->ptr[0]);
                    c->ptr++;
                }
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                *p++ = (char)n;
            } else if (c->ptr[0] == 'n') {
                /* "\n" */
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                *p++ = '\n';
                c->ptr++;
            } else if ((c->ptr[0] == ' ') || (c->ptr[0] == '\n')) {
                /* ignore escaped charcter.  '\ ' -> '' */
                /* usually used as the end of escaped charaters */
                c->ptr++;
            } else {
                if (buffer_used++ == READER_BUFSIZE) { return LISP_ERROR(Memory_Error); }
                /* the character itself.  '\W' -> 'W' */
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
    if (++(c->nesting_level) > READER_MAX_NESTING_LEVEL) {
        return LISP_ERROR(Reader_Error);
    }
    size_t buffer_used = 0;
    c->ptr++;
    for (size_t i = 0; i < READER_ARRAY_BUFSIZE; i++) {
        reader_skip_spaces_and_comment(c);
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
    reader_skip_spaces_and_comment(c);
    switch (c->ptr[0]) {
    case '\0':
        return LISP_ERROR(Reader_Error);
    case '\'':
        return reader_quoted(c);
    case '(':
        return reader_list(c);
    case '\"':
        return reader_string(c);
    case '?':
        return reader_char(c);
    case '#':
        return reader_hashnum(c);
    default:
        return reader_maybe_symbol(c);
    }
}

Lisp_Object reader(const char *str, lispenv_t *env) {
    reader_context c = (reader_context){ .ptr = str, .nesting_level = 0, .env = env };
    return reader_sexp(&c);
}
