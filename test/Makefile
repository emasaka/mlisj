CC := gcc
ifeq ($(CC),gcc)
  CFLAGS := -O3 -W -Wall -fsanitize=address -static-libasan
else
  CFLAGS := -O3 -W -Wall -fsanitize=address
endif
LDFLAGS := -lcunit

TEST_EXECS := test_mempool test_symbol test_variable test_functable test_lispenv test_reader test_eval test_func-simple test_func-skk-gadgets test_util
OBJS := mempool.o symbol.o variable.o functable.o lispenv.o reader.o eval.o func-simple.o func-skk-gadgets.o util.o writer.o

.DEFAULT_GOAL := test

.PHONY: test clean

test: $(TEST_EXECS)
	$(foreach CMD, $(TEST_EXECS), ./$(CMD) &&) true

# Test executables

%: %.c
	$(CC) $< $(filter %.o,$^) $(CFLAGS) $(LDFLAGS) -o $@

LISPENV_H_AND_DEPS := ../src/lispenv.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h
LISPENV_O_AND_DEPS :=  lispenv.o mempool.o symbol.o variable.o functable.o

test_mempool: test_mempool.c ../src/mempool.h mempool.o ../src/lispobject.h ../src/mempool.h

test_symbol: test_symbol.c ../src/symbol.h symbol.o mempool.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h

test_variable: test_variable.c ../src/variable.h variable.o mempool.o symbol.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h

test_functable: test_functable.c ../src/functable.h functable.o mempool.o symbol.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/functable.h

test_lispenv: test_lispenv.c $(LISPENV_O_AND_DEPS) $(LISPENV_H_AND_DEPS)

test_reader: test_reader.c reader.o util.o writer.o $(LISPENV_O_AND_DEPS) helper/writer.h $(LISPENV_H_AND_DEPS)

test_eval: test_eval.c eval.o reader.o util.o writer.o $(LISPENV_O_AND_DEPS) ../src/eval.h ../src/reader.h helper/writer.h $(LISPENV_H_AND_DEPS)

test_func-simple: test_func-simple.c func-simple.o eval.o reader.o util.o $(LISPENV_O_AND_DEPS) ../src/eval.h ../src/reader.h $(LISPENV_H_AND_DEPS)

test_func-skk-gadgets: test_func-skk-gadgets.c func-skk-gadgets.o eval.o reader.o util.o writer.o $(LISPENV_O_AND_DEPS) ../src/eval.h ../src/reader.h $(LISPENV_H_AND_DEPS)

test_util: test_util.c util.o ../src/util.h

# Object modules

%.o: ../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

mempool.o: ../src/mempool.c ../src/lispobject.h ../src/mempool.h

symbol.o: ../src/symbol.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h

variable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h

functable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/functable.h

lispenv.o: ../src/lispenv.c $(LISPENV_H_AND_DEPS)

reader.o: ../src/reader.c ../src/util.h $(LISPENV_H_AND_DEPS)

eval.o: ../src/eval.c  ../src/eval.h ../src/reader.h ../src/func-helper.h $(LISPENV_H_AND_DEPS)

func-simple.o: ../src/func-simple.c ../src/eval.h ../src/reader.h ../src/util.h ../src/func-helper.h $(LISPENV_H_AND_DEPS)

func-skk-gadgets.o: ../src/func-skk-gadgets.c ../src/eval.h ../src/reader.h ../src/util.h ../src/func-helper.h ../src/util.h $(LISPENV_H_AND_DEPS)

util.o: ../src/util.c ../src/util.h

writer.o: helper/writer.c ../src/lispobject.h ../src/mempool.h
	$(CC) -c $(CFLAGS) $< -o $@

# other

clean:
	rm -f $(TEST_EXECS) $(OBJS)
