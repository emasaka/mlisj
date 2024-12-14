#!/usr/bin/env ruby
# mlisj
# Copyright (C) 2023-2024 emasaka
# This program comes with ABSOLUTELY NO WARRANTY.
# This is free software, and you are welcome to redistribute it
# under certain conditions.

require 'fiddle'
require 'fiddle/import'

## load libmlisj
module Mlisj
  extend Fiddle::Importer
  dlload '../src/libmlisj.so'
  extern 'int mlisj_eval(const char *, char *, size_t, char **, const char *)'
  StrP = struct(['void *ptr'])

  module_function
  def strp_array_set(ary, idx, val)
    StrP.new(ary + (idx * Fiddle::SIZEOF_VOIDP)).ptr = val
  end
end

MLISJ_ERROR_EVALUATION = 1
MLISJ_ERROR_RUNTIME = 2

## function to call mlisj_eval()
BUFSIZE = 256

def make_c_strary(strlist)
  return nil if strlist.nil?
  ary = Fiddle::Pointer.malloc(Fiddle::SIZEOF_VOIDP * (strlist.size + 1), Fiddle::RUBY_FREE)
  strlist.each_with_index do |str, i|
    Mlisj::strp_array_set(ary, i, Fiddle::Pointer[str])
  end
  Mlisj::strp_array_set(ary, strlist.size, Fiddle::NULL)
  ary
end

def call_mlisj(str, skk_num_list = nil, henkan_key = nil)
  puts str

  c_str = Fiddle::Pointer[str]
  buffer = Fiddle::Pointer.malloc(BUFSIZE, Fiddle::RUBY_FREE)
  c_skk_num_list = make_c_strary(skk_num_list)
  c_henkan_key = henkan_key ? Fiddle::Pointer[henkan_key] : nil

  ret = Mlisj::mlisj_eval(c_str, buffer, BUFSIZE, c_skk_num_list, c_henkan_key)
  case ret
  when 0
    outstr = buffer.to_s
    puts "-> #{outstr}"
  when MLISJ_ERROR_EVALUATION
    puts "-> #{str}"
  else
    puts "-> error!"
  end
end

## main
call_mlisj("(笑)")
call_mlisj("(concat \"I\057O\")")
call_mlisj("(symbol-value 'user-full-name)")
call_mlisj("(make-string (- (window-width) 5) ?-)")
call_mlisj("(current-time-string)")
call_mlisj("(substring (current-time-string) 11 16)")
call_mlisj("(skk-current-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)))")
call_mlisj("(skk-relative-date (lambda (date-information format gengo and-time) (skk-default-current-date date-information \"%s-%s-%s(%s)\" 0 nil 0 0 nil)) nil nil :dd -1)")
call_mlisj("(skk-times)", ["3", "5"]);
call_mlisj("(skk-gengo-to-ad \"\" \"年\")", ["60"], "しょうわ60ねん")
call_mlisj("(skk-gadget-units-conversion \"inch\" (string-to-number (car skk-num-list)) \"cm\")", ["10"])
