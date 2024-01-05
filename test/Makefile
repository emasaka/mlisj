TESTDIR := test
CFLAGS := -W -Wall

.DEFAULT_GOAL := test

.PHONY: test clean

test: test_mempool test_symbol test_reader
	./test_mempool
	./test_symbol
	./test_reader

test_mempool: test_mempool.c ../src/mempool.c
	$(CC) test_mempool.c $(CFLAGS) -lcunit -o test_mempool

test_symbol: test_symbol.c ../src/mempool.c mempool.o
	$(CC) test_symbol.c mempool.o $(CFLAGS) -lcunit -o test_symbol

test_reader: test_reader.c ../src/reader.c mempool.o symbol.o writer.o
	$(CC) test_reader.c mempool.o symbol.o writer.o $(CFLAGS) -lcunit -o test_reader

mempool.o: ../src/mempool.c ../src/mempool.h
	$(CC) -c ../src/mempool.c $(CFLAGS) -o mempool.o

symbol.o: ../src/symbol.c ../src/symbol.h
	$(CC) -c ../src/symbol.c $(CFLAGS) -o symbol.o

writer.o: writer.c ../src/lispobject.h
	$(CC) -c writer.c $(CFLAGS) -o writer.o

clean:
	rm -f test_mempool test_symbol test_reader mempool.o symbol.o writer.o
