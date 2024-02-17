TESTDIR := test
CC := gcc
CFLAGS := -W -Wall -fsanitize=address -static-libasan
TEST_CFLAGS := $(CFLAGS) -lcunit

TEST_EXECS := test_mempool test_symbol test_variable test_functable test_reader
OBJS := mempool.o symbol.o variable.o functable.o lispenv.o writer.o

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

test_reader: test_reader.c ../src/reader.c mempool.o symbol.o variable.o functable.o lispenv.o writer.o ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h ../src/lispenv.h

# Object modules

%.o: ../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

mempool.o: ../src/mempool.c ../src/lispobject.h ../src/mempool.h

symbol.o: ../src/symbol.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h

variable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h

functable.o: ../src/variable.c ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/functable.h

lispenv.o: ../src/lispenv.c ../src/lispenv.h ../src/lispobject.h ../src/mempool.h ../src/symbol.h ../src/variable.h ../src/functable.h

writer.o: writer.c ../src/lispobject.h ../src/mempool.h
	$(CC) -c $(CFLAGS) $< -o $@

# other

clean:
	rm -f $(TEST_EXECS) $(OBJS)
