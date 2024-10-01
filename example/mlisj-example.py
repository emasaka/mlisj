#!/usr/bin/env python3
import ctypes

## load libmlisj
libmlisj = ctypes.CDLL('../src/libmlisj.so')
mlisj_eval = libmlisj.mlisj_eval
mlisj_eval.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_char_p), ctypes.c_char_p]
mlisj_eval.restype = ctypes.c_int

MLISJ_ERROR_EVALUATION = 1
MLISJ_ERROR_RUNTIME = 2

## function to call mlisj_eval()
BUFSIZE = 256

def make_c_strary(strlist):
    if strlist is None:
        return None
    ary = (ctypes.c_char_p * (len(strlist) + 1))()
    ary[:-1] = [x.encode() for x in strlist]
    ary[-1] = None
    return ary

def call_mlisj(str, skk_num_list):
    print(str)
    c_str = str.encode()
    buffer = ctypes.create_string_buffer(BUFSIZE)
    c_skk_num_list = make_c_strary(skk_num_list)
    ret = mlisj_eval(c_str, buffer, BUFSIZE, c_skk_num_list, None)
    if ret == 0:
        outstr = buffer.value.decode()
        print("-> %s" % outstr)
    elif ret == MLISJ_ERROR_EVALUATION:
        print("-> %s" % str);
    else:
        print("-> error!")

## main
call_mlisj("(笑)", None)
call_mlisj("(concat \"I\057O\")", None)
call_mlisj("(symbol-value 'user-full-name)", None)
call_mlisj("(make-string (- (window-width) 5) ?-)", None)
call_mlisj("(current-time-string)", None)
call_mlisj("(substring (current-time-string) 11 16)", None)
call_mlisj("(skk-current-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)))", None)
call_mlisj("(skk-relative-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)) nil nil :dd -1)", None)
call_mlisj("(skk-times)", ["3", "5"])
