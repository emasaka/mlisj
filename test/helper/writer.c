#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../src/lispobject.h"
#include "../../src/mempool.h"

#define WRITER_BUFSIZE 512
#define TMP_BUFFSIZE 128

typedef struct {
    char *ptr;
    size_t count;
} writer_context;

int writer_puts(char *str, writer_context *c) {
    size_t len = strlen(str);
    if (len + c->count < WRITER_BUFSIZE) {
        strcpy(c->ptr, str);
        c->ptr += len;
        c->count += len;
        return 0;
    } else {
        return -1;
    }
}

int writer_putc(int ch, writer_context *c) {
    if (c->count + 2 < WRITER_BUFSIZE) {
        c->ptr[0] = ch;
        c->ptr[1] = '\0';
        c->ptr++;
        c->count++;
        return 0;
    } else {
        return -1;
    }
}

int writer_int(Lisp_Object obj, writer_context *c) {
    char tmp_buf[TMP_BUFFSIZE];
    // coopying twide is not efficient but easy
    if (snprintf(tmp_buf, TMP_BUFFSIZE, "%d", obj.val.ival) >= 0) {
        return writer_puts(tmp_buf, c);
    } else {
        return -1;
    }
}

int writer_symbol(Lisp_Object obj, writer_context *c) {
    return writer_puts(obj.val.sval, c);
}

int writer_nil(writer_context *c) {
    return writer_puts("nil", c);
}

int writer_string(Lisp_Object obj, writer_context *c) {
    char tmp_buf[TMP_BUFFSIZE];
    unsigned char *p = (unsigned char *)obj.val.sval;
    int ch;
    int ret;
    ret = writer_putc('"', c);
    if (ret != 0) { return ret; }
    while ((ch = *p++) != '\0') {
        if (isprint(ch) || ch > 0x7f) {
            // WORKAROUND: assume a character larger than 0x7f is a part of multibyte character
            if (ch == '\"') {
                ret = writer_putc('\\', c);
                if (ret != 0) { return ret; }
            }
            ret = writer_putc(ch, c);
            if (ret != 0) { return ret; }
        } else {
            if (snprintf(tmp_buf, TMP_BUFFSIZE, "%03o", ch) >= 0) {
                ret = writer_puts("\\", c);
                if (ret != 0) { return ret; }
                ret = writer_puts(tmp_buf, c);
                if (ret != 0) { return ret; }
            } else {
                return -1;
            }
        }
    }
    ret = writer_putc('"', c);
    if (ret != 0) { return ret; }
    return 0;
}

int writer_sexp(Lisp_Object, writer_context *); /* prototype declaration */

int writer_clist(Lisp_Object obj, writer_context *c) {
    NArray *ary = obj.val.aval;
    int ret;
    ret = writer_putc('(', c);
    if (ret != 0) { return ret; }
    for (size_t i = 0; i < ary->size; i++) {
        ret = writer_sexp(ary->data[i], c);
        if (ret != 0) { return ret; }
        if ((i + 1) < ary->size) {
            ret = writer_putc(' ', c);
            if (ret != 0) { return ret; }
        }
    }
    return writer_putc(')', c);
}

int writer_sexp(Lisp_Object obj, writer_context *c) {
    switch (obj.type) {
    case Lisp_Symbol:
        return writer_symbol(obj, c);
    case Lisp_Int:
        return writer_int(obj, c);
    case Lisp_Float:
        return -1;      // NOT IMPLEMENTED
    case Lisp_String:
        return writer_string(obj, c);
    case Lisp_CList:
        return writer_clist(obj, c);
    case Lisp_Nil:
        return writer_nil(c);
    default:
        return -1;
    }
}

int writer(Lisp_Object obj, char *buffer) {
    writer_context c = (writer_context){ .ptr = buffer, .count = 0 };
    return writer_sexp(obj, &c);
}
