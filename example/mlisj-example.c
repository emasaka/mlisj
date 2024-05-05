#include <stdio.h>
#include "../src/mlisj.h"

#define BUFSIZE 256

void call_mlisj(char *str, char **skk_num_list) {
    char buffer[BUFSIZE];

    puts(str);
    int ret = mlisj_eval(str, buffer, BUFSIZE, skk_num_list, NULL);
    if (ret == 0) {
	printf("-> %s\n", buffer);
    } else if (ret == MLISJ_ERROR_EVALUATION) {
	printf("-> %s\n", str);
    } else {
	puts("-> error!");
    }
}

int main() {
    call_mlisj("(笑)", NULL);
    call_mlisj("(concat \"I\057O\")", NULL);
    call_mlisj("(symbol-value 'user-full-name)", NULL);
    call_mlisj("(make-string (- (window-width) 5) ?-)", NULL);
    call_mlisj("(current-time-string)", NULL);
    call_mlisj("(substring (current-time-string) 11 16)", NULL);
    call_mlisj("(skk-current-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)))", NULL);
    call_mlisj("(skk-relative-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)) nil nil :dd -1)", NULL);

    char *num_list[] = { "3", "5", NULL };
    call_mlisj("(skk-times)", num_list);

    return 0;
}
