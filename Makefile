CC := gcc

.DEFAULT_GOAL := lib

.PHONY: lib
lib:
	cd src; $(MAKE) CC=$(CC)

.PHONY: test
test:
	cd test; $(MAKE) test CC=$(CC)

.PHONY: clean
clean:
	cd src; $(MAKE) clean
	cd test; $(MAKE) clean
