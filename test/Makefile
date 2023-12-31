TESTDIR := test
CFLAGS := -W -Wall

.DEFAULT_GOAL := test

.PHONY: test

test: test_mempool
	./test_mempool

test_mempool: test_mempool.c ../src/mempool.c
	$(CC) test_mempool.c $(CFLAGS) -lcunit -o test_mempool

clean:
	rm -f test_mempool
