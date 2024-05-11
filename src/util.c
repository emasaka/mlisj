#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"

/*
    check if the string is a number
*/
check_num_t check_num_str(char *str, bool num_only_p) {
    /* XXX: exponential notation of float is not supported */

    enum { START, DIGITS, DOT, DIGITS_AND_DOT } state = START;

    char *p = str;
    if (*p == '\0') {
        return R_NOTNUM;
    } else if (*p == '-' || *p == '+') {
        p++;
    }

    while (true) {
        switch (state){
        case START:
            if (isdigit(*p)) {
                state = DIGITS;
            } else if (*p == '.') {
                state = DOT;
            } else {
                return R_NOTNUM;
            }
            break;
        case DIGITS:
            if (isdigit(*p)) {
                state = DIGITS;
            } else if (*p == '.') {
                state = DIGITS_AND_DOT;
            } else {
                if (num_only_p == true &&  *p != '\0') {
                    return R_NOTNUM;
                } else {
                    return R_INT;
                }
            }
            break;
        case DOT:
            if (isdigit(*p)) {
                state = DIGITS_AND_DOT;
            } else {
                return R_NOTNUM;
            }
            break;
        case DIGITS_AND_DOT:
            if (isdigit(*p)) {
                state = DIGITS_AND_DOT;
            } else {
                if (num_only_p == true &&  *p != '\0') {
                    return R_NOTNUM;
                } else {
                    return R_FLOAT;
                }
            }
            break;
        }
        p++;
    }
}

/*
    skk_num_exp (like skk-num.el)
    convert a hankaku number string to zenkaku.
*/

static char *zenkaku_digits_list[] = {
    "０", "１", "２", "３", "４", "５", "６", "７", "８", "９"
};

#define ZENKAKU_DIGIT_STRLEN (sizeof("０") - 1)

int skk_num_type1_kanji(const char *src, char *dst, size_t size) {
    const char *s = src;
    char *d = dst;
    size_t i = 0;
    while (*s != '\0') {
        if (isdigit(*s)) {
            if (i + ZENKAKU_DIGIT_STRLEN >= size) { return -1; }
            strcpy(d, zenkaku_digits_list[*s - '0']);
            d += ZENKAKU_DIGIT_STRLEN;
            s++;
            i += ZENKAKU_DIGIT_STRLEN;
        } else {
            if (i + 1 >= size) { return -1; }
            *d++ = *s++;
            i++;
        }
    }
    *d = '\0';
    return 0;
}

static char *kansuji_digits_list[] = {
    "〇","一","二","三", "四","五","六","七", "八","九"
};

static char *japanese_keta_4[] = {
    "", "十", "百", "千"
};

static void skk_num_to_kanji_4digits(const char *src, char *dst) {
    size_t len = strlen(src);
    const char *s = src;
    char *d = dst;
    while (*s != '\0') {
        char c = *s++;
        len--;
        if (c == '0') {
            continue;
        } else if (!(c == '1' && len > 0)) {
            strcpy(d, kansuji_digits_list[c - '0']);
            d += ZENKAKU_DIGIT_STRLEN;
        }
        char *keta = japanese_keta_4[len];
        strcpy(d, keta);
        d += strlen(keta);
    }
    *d = '\0';
}

static void str_prepend(char *base, const char * newstr) {
    size_t len = strlen(newstr);
    if (len > 0) {
        memmove(base + len, base, strlen(base) + 1);
        memcpy(base, newstr, len);
    }
}

static char *japanese_keta_man[] = {
    "", "万", "億", "兆", "京"
};

int skk_num_type3_kanji(const char *src, char *dst, size_t size) {
    char buff4_out[ZENKAKU_DIGIT_STRLEN * 8 + 1];
    char buff4[5];
    buff4[4] = '\0';
    size_t keta = 0;
    size_t slen = strlen(src);
    dst[0] = '\0';
    while (keta < (sizeof(japanese_keta_man) / sizeof(japanese_keta_man[0]))) {
        if (slen > 4) {
            strncpy(buff4, src + (slen - 4), 4);
            slen -= 4;
        } else {
            strncpy(buff4, src, slen);
            buff4[slen] = '\0';
            slen = 0;
        }
        skk_num_to_kanji_4digits(buff4, buff4_out);
        if (buff4_out[0] != '\0') {
            char *keta_man = japanese_keta_man[keta];
            if (strcmp(buff4_out, "千") == 0 && keta != 0) {
                strcpy(buff4_out, "一千");
            }
            strcat(buff4_out, keta_man);
            if (strlen(dst) + strlen(buff4_out) > size) { return -1; }
            str_prepend(dst, buff4_out);
        }
        if (slen == 0) {
            /* exit function */
            if (dst[0] == '\0') {
                if (strlen(dst) + strlen(kansuji_digits_list[0]) > size) { return -1; }
                strcpy(dst, kansuji_digits_list[0]);
            }
            return 0;
        }
        keta++;
    }
    /* too big number */
    return -1;
}

int skk_num_type0_kanji(const char *src, char *dst, size_t size) {
    /* just copy */
    strncpy(dst, src, size);
    dst[size - 1] = '\0';
    return 0;
}

typedef int(*skk_num_type_func_t)(const char *, char *, size_t);

static skk_num_type_func_t skk_num_type_list[] = {
    skk_num_type0_kanji,  /* 0 */
    skk_num_type1_kanji,  /* 1 */
    NULL, /* 2: not implemented */
    skk_num_type3_kanji,  /* 3 */
    NULL, /* 4: not implemented */
    NULL, /* 5: not implemented */
    NULL, /* 6: not implemented */
    NULL, /* 7: not implemented */
    NULL, /* 8: not implemented */
    NULL  /* 9: not implemented */
};

int skk_num_exp(const char *num, size_t type_idx, char *dst, size_t size) {
    if (type_idx > (sizeof(skk_num_type_list) / sizeof(skk_num_type_list[0]))) {
        return -1;
    }
    skk_num_type_func_t func = skk_num_type_list[type_idx];
    if (func == NULL) {
        return -1;
    }
    return func(num, dst, size);
}
