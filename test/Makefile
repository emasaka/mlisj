TESTDIR := test
CFLAGS := -W -Wall
TEST_CFLAGS := $(CFLAGS) -lcunit

TEST_EXECS := test_mempool test_symbol test_reader test_variable test_functable
OBJS := mempool.o symbol.o writer.o

.DEFAULT_GOAL := test

.PHONY: test clean

test: $(TEST_EXECS)
	$(shell echo $(TEST_EXECS) | sed -e 's| | \&\& ./|g;s|^|./|')
# foo bar baz -> ./foo && ./bar && ./baz

test_mempool: test_mempool.c ../src/mempool.c
	$(CC) $< $(TEST_CFLAGS) -o $@

test_symbol: test_symbol.c ../src/mempool.c mempool.o
	$(CC) $< mempool.o $(TEST_CFLAGS) -o $@

test_reader: test_reader.c ../src/reader.c mempool.o symbol.o writer.o
	$(CC) $< mempool.o symbol.o writer.o $(TEST_CFLAGS) -o $@

test_variable: test_variable.c ../src/variable.c mempool.o symbol.o
	$(CC) $< mempool.o symbol.o $(TEST_CFLAGS) -o $@

test_functable: test_functable.c ../src/functable.c mempool.o symbol.o
	$(CC) $< mempool.o symbol.o $(TEST_CFLAGS) -o $@

mempool.o: ../src/mempool.c ../src/mempool.h
	$(CC) -c $(CFLAGS) $< -o $@

symbol.o: ../src/symbol.c ../src/symbol.h
	$(CC) -c $(CFLAGS) $< -o $@

writer.o: writer.c ../src/lispobject.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TEST_EXECS) $(OBJS)
