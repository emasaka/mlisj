#include <ctype.h>
#include <string.h>
#include "lispobject.h"
#include "mempool.h"
#include "symbol.h"

#define READER_BUFSIZE 512

typedef struct {
    char *ptr;
} reader_context;

Lisp_Object reader_sexp(reader_context *); /* prototype declaration */

#define READER_ERROR_VAL ((Lisp_Object){ .type = Internal_Error, .val.err = Reader_Error })
#define MEMORY_ERROR_VAL ((Lisp_Object){ .type = Internal_Error, .val.err = Memory_Error })
#define NIL_VAL ((Lisp_Object) { .type = Lisp_Nil, .val.ival = 0 })

/* workaround: "-#0" -> (- #0) */
Lisp_Object reader_minus_hash(char *str) {
    char *sym = str2symbol(str + 1, true);
    if (sym == NULL ) { return MEMORY_ERROR_VAL; }
    NArray *ary = new_narray(2);
    if (ary == NULL ) { return MEMORY_ERROR_VAL; }
    ary->data[0] = (Lisp_Object){ .type = Lisp_Symbol, .val.sval = Symbol_minus };
    ary->data[1] = (Lisp_Object){ .type = Lisp_Symbol, .val.sval = sym };
    return (Lisp_Object){ .type = Lisp_CList, .val.aval = ary };
}

/* parse number, or return nil */
Lisp_Object reader_check_number(char *str) {
    /* XXX: only dicimal integer (and num-list) is supported */
    int minus = 1;
    char *p = str;
    if (str[0] == '-') {
        if (str[1] == '#') {
            /* example: "-#0" */
            return reader_minus_hash(str);
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
            return NIL_VAL;
        }
    }
    return (Lisp_Object){ .type = Lisp_Int, .val.ival = n * minus };
}

Lisp_Object reader_maybe_symbol(reader_context *c) {
    char buffer[READER_BUFSIZE];
    char *p = buffer;
    for (int i = 0; i < READER_BUFSIZE; i++) {
        char ch = c->ptr[0];
        /* XXX: escapes in symbol is not supported */
        if (isspace(ch) || (ch == '(') || (ch == ')') || (ch == '\"') || (ch == '\'') || (ch == '\0')) {
            *p = '\0';

            /* number? */
            Lisp_Object n = reader_check_number(buffer);
            if (n.type != Lisp_Nil) {
                return n;
            }

            /* nil? */
            if (strcmp(buffer, "nil") == 0) {
                return NIL_VAL;
            }

            /* returns a symbol */
            char *sym = str2symbol(buffer, true);
            if (sym == NULL) {
                return MEMORY_ERROR_VAL;
            } else {
                return (Lisp_Object){ .type = Lisp_Symbol, .val.sval = sym };
            }
        } else {
            *p++ = ch;
            c->ptr++;
        }
    }
    /* too long */
    return READER_ERROR_VAL;
}

Lisp_Object reader_char(reader_context *c) {
    /* XXX: escapes in character is not supported */
    /* XXX: multibyte character is not supported */
    c->ptr++;
    if (*(c->ptr) == '\0') { return READER_ERROR_VAL; }
    return (Lisp_Object){ .type = Lisp_Int, .val.ival = *(c->ptr++) };
}

Lisp_Object reader_string(reader_context *c) {
    /* XXX: "\x<hex>" and "\u<code>" are not supported */
    /* XXX: escaping multibyte character is not supported (maybe no problem) */
    c->ptr++;
    char buffer[READER_BUFSIZE];
    int buffer_used = 0;
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
                if (buffer_used++ == READER_BUFSIZE) { return MEMORY_ERROR_VAL; }
                *p++ = (char)n;
            } else {
                if (buffer_used++ == READER_BUFSIZE) { return MEMORY_ERROR_VAL; }
                *p++ = *(c->ptr++);
            }
            break;
        case '\"':
            if (buffer_used == READER_BUFSIZE) { return MEMORY_ERROR_VAL; }
            *p = '\0';
            c->ptr++;
            char *newstr = copy_to_string_area(buffer);
            if (newstr == NULL ) { return MEMORY_ERROR_VAL; }
            return (Lisp_Object){ .type = Lisp_String, .val.sval = newstr };
            break;
        default:
            if (buffer_used++ == READER_BUFSIZE) { return MEMORY_ERROR_VAL; }
            *p++ = *(c->ptr++);
            break;
        }
    }
    /* double-quote is not closed */
    return READER_ERROR_VAL;
}

Lisp_Object reader_list(reader_context *c) {
    Lisp_Object buffer[READER_BUFSIZE];
    int buffer_used = 0;
    c->ptr++;
    for (int i = 0; i < READER_BUFSIZE; i++) {
        while (isspace(c->ptr[0])) { c->ptr++; }
        if (c->ptr[0] == ')') {
            c->ptr++;
            NArray *ary = new_narray(buffer_used);
            if (ary == NULL ) { return MEMORY_ERROR_VAL; }
            for (int j = 0; j < buffer_used; j++) {
                ary->data[j] = buffer[j];
            }
            ary->size = buffer_used;
            return (Lisp_Object){ .type = Lisp_CList, .val.aval = ary};
        }

        Lisp_Object v = reader_sexp(c);
        if (v.type == Internal_Error ) { return v; }
        if (buffer_used == READER_BUFSIZE)  { return MEMORY_ERROR_VAL; }
        buffer[buffer_used++] = v;
    }
    /* too long */
    return READER_ERROR_VAL;
}

Lisp_Object reader_quoted(reader_context *c) {
    c->ptr++;
    Lisp_Object v = reader_sexp(c);
    if (v.type == Internal_Error ) { return v; }
    NArray *ary = new_narray(2);
    if (ary == NULL ) { return MEMORY_ERROR_VAL; }
    ary->data[0] = (Lisp_Object){ .type = Lisp_Symbol, .val.sval = Symbol_quote};
    ary->data[1] = v;
    return (Lisp_Object){ .type = Lisp_CList, .val.aval = ary };
}

Lisp_Object reader_sexp(reader_context *c) {
    while (isspace(c->ptr[0])) { c->ptr++; }
    if (c->ptr[0] == '\0') {
        return READER_ERROR_VAL;
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

Lisp_Object reader(char *str) {
    reader_context c = (reader_context){ .ptr = str };
    return reader_sexp(&c);
}
