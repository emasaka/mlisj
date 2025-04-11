/*
  mlisj
  Copyright (C) 2023-2024 emasaka
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions.
*/

#include <stdio.h>
#include "../src/mlisj.h"

#define BUFSIZE 256

void call_mlisj(char *str, char **skk_num_list, char *henkan_key) {
    char buffer[BUFSIZE];

    puts(str);
    int ret = mlisj_eval(str, buffer, BUFSIZE, skk_num_list, henkan_key);
    if (ret == 0) {
        printf("-> %s\n", buffer);
    } else if (ret == MLISJ_ERROR_EVALUATION) {
        printf("-> %s\n", str);
    } else {
        puts("-> error!");
    }
}

int main() {
    call_mlisj("(笑)", NULL, NULL);
    call_mlisj("(concat \"I\\057O\")", NULL, NULL);
    call_mlisj("(symbol-value 'user-full-name)", NULL, NULL);
    call_mlisj("(make-string (- (window-width) 5) ?-)", NULL, NULL);
    call_mlisj("(current-time-string)", NULL, NULL);
    call_mlisj("(substring (current-time-string) 11 16)", NULL, NULL);
    call_mlisj("(skk-current-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)))", NULL, NULL);
    call_mlisj("(skk-relative-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)) nil nil :dd -1)", NULL, NULL);

    char *num_list1[] = { "3", "5", NULL };
    call_mlisj("(skk-times)", num_list1, NULL);

    char *num_list2[] = { "60", NULL };
    call_mlisj("(skk-gengo-to-ad \"\" \"年\")", num_list2, "しょうわ60ねん");

    char *num_list3[] = { "10", NULL };
    call_mlisj("(skk-gadget-units-conversion \"inch\" (string-to-number (car skk-num-list)) \"cm\")", num_list3, NULL);

    return 0;
}
