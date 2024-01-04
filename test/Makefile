TESTDIR := test
CFLAGS := -W -Wall

.DEFAULT_GOAL := test

.PHONY: test clean

test: test_mempool test_symbol
	./test_mempool
	./test_symbol

test_mempool: test_mempool.c ../src/mempool.c
	$(CC) test_mempool.c $(CFLAGS) -lcunit -o test_mempool

test_symbol: test_symbol.c ../src/mempool.c mempool.o
	$(CC) test_symbol.c mempool.o $(CFLAGS) -lcunit -o test_symbol

mempool.o: ../src/mempool.c ../src/mempool.h
	$(CC) -c ../src/mempool.c $(CFLAGS) -o mempool.o

clean:
	rm -f test_mempool test_symbol mempool.o
