TESTDIR := test
CC := gcc
CFLAGS := -W -Wall -fsanitize=address -static-libasan
TEST_CFLAGS := $(CFLAGS) -lcunit

TEST_EXECS := test_mempool test_symbol test_variable test_functable test_lispenv test_reader test_eval test_func-simple
OBJS := mempool.o symbol.o variable.o functable.o lispenv.o reader.o eval.o func-simple.o writer.o

.DEFAULT_GOAL := test

.PHONY: test clean

test: $(TEST_EXECS)
	$(shell echo $(TEST_EXECS) | sed -e 's| | \&\& ./|g;s|^|./|')
# foo bar baz -> ./foo && ./bar && ./baz

# Test executables

%: %.c
	$(CC) $< $(filter %.o,$^) $(TEST_CFLAGS) -o $@

test_mempool: test_mempool.c ../src/mempool.c ../src/lispobject.h ../src/mempool.h

test_symbol: test_symbol.c ../src/symbol.c mempool.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h

test_variable: test_variable.c ../src/variable.c mempool.o symbol.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h

test_functable: test_functable.c ../src/functable.c mempool.o symbol.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/functable.h

test_lispenv: ../src/lispenv.c mempool.o symbol.o variable.o functable.o func-simple.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h

test_reader: test_reader.c reader.o mempool.o symbol.o variable.o functable.o lispenv.o func-simple.o writer.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h

test_eval: test_eval.c eval.o reader.o mempool.o symbol.o variable.o functable.o lispenv.o func-simple.o writer.o ../src/eval.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h ../src/reader.h ../src/eval.h

test_func-simple: test_func-simple.c func-simple.o eval.o reader.o mempool.o symbol.o variable.o functable.o lispenv.o eval.o writer.o ../src/eval.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h ../src/reader.h ../src/eval.h

# Object modules

%.o: ../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

mempool.o: ../src/mempool.c ../src/lispobject.h ../src/mempool.h

symbol.o: ../src/symbol.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h

variable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h

functable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/functable.h

lispenv.o: ../src/lispenv.c ../src/lispenv.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h

reader.o: ../src/reader.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h

eval.o: ../src/eval.c  ../src/eval.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h ../src/reader.h

func-simple.o: ../src/func-simple.c ../src/eval.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h ../src/reader.h

writer.o: helper/writer.c ../src/lispobject.h ../src/mempool.h
	$(CC) -c $(CFLAGS) $< -o $@

# other

clean:
	rm -f $(TEST_EXECS) $(OBJS)
