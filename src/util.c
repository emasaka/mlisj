/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"

/*
    MARK: check_num_str()
    check if the string is a number
*/
check_num_t check_num_str(const char *str, bool num_only_p) {
    /* NOTE: exponential notation of float is not supported */

    enum { START, DIGITS, DOT, DIGITS_AND_DOT } state = START;

    const char *p = str;
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
    MARK: skk_num_exp()
    routines like skk-num.el
    convert a hankaku number string to zenkaku.
*/

static const char * const zenkaku_digits_list[] = {
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

static const char * const kansuji_digits_list[] = {
    "〇","一","二","三", "四","五","六","七", "八","九"
};

static const char * const japanese_keta_4[] = {
    "", "十", "百", "千"
};

static void skk_num_to_kanji_4digits(const char *src, char *dst) {
    size_t len = strlen(src);
    if (len > 4) { len = 4; }
    const char *s = src;
    char *d = dst;
    while (*s != '\0') {
        if (len == 0) { break; }
        char c = *s++;
        len--;
        if (c == '0') {
            continue;
        } else if (!(c == '1' && len > 0)) {
            strcpy(d, kansuji_digits_list[c - '0']);
            d += ZENKAKU_DIGIT_STRLEN;
        }
        const char *keta = japanese_keta_4[len];
        strcpy(d, keta);
        d += strlen(keta);
    }
    *d = '\0';
}

static const char * const japanese_keta_man[] = {
    "", "万", "億", "兆", "京"
};

int skk_num_type3_kanji(const char *src, char *dst, size_t size) {
    char buff4_out[ZENKAKU_DIGIT_STRLEN * 8 + 1];
    char buff4[5];
    size_t slen = strlen(src);
    size_t ketalen = slen / 4;
    size_t ketawin = slen % 4;

    if (ketawin == 0) {
        ketawin = 4;
        ketalen--;
    }
    if (ketalen > (sizeof(japanese_keta_man) / sizeof(japanese_keta_man[0]))) {
        /* too big number */
        return -1;
    }

    dst[0] = '\0';
    size_t dlen = 0;
    const char *sp = src;
    char *dp = dst;
    for (int keta = ketalen; keta >= 0; keta--) {
        memcpy(buff4, sp, ketawin);
        buff4[ketawin] = '\0';

        skk_num_to_kanji_4digits(buff4, buff4_out);
        if (buff4_out[0] != '\0') {
            const char *keta_man = japanese_keta_man[keta];
            if (strcmp(buff4_out, "千") == 0 && keta != 0) {
                strcpy(buff4_out, "一千");
            }
            size_t buff4_out_len = strlen(buff4_out);
            size_t keta_man_len = strlen(keta_man);
            if (dlen + buff4_out_len + keta_man_len > size) { return -1; }
            memcpy(dp, buff4_out, buff4_out_len);
            dp += buff4_out_len;
            memcpy(dp, keta_man, keta_man_len);
            dp += keta_man_len;
            dp[0] = '\0';
            dlen = dlen + buff4_out_len + keta_man_len;
        }

        sp += ketawin;
        ketawin = 4;
    }

    if (dlen == 0) {
        if (strlen(kansuji_digits_list[0]) > size) { return -1; }
        strcpy(dst, kansuji_digits_list[0]);
    }
    return 0;
}

int skk_num_type0_kanji(const char *src, char *dst, size_t size) {
    /* just copy */
    size_t src_size = strlen(src) + 1;
    if (src_size <= size) {
        memcpy(dst, src, src_size);
    } else {
        return -1;
    }
    return 0;
}

typedef int(*skk_num_type_func_t)(const char *, char *, size_t);

static const skk_num_type_func_t skk_num_type_list[] = {
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
