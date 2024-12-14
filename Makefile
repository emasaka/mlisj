CC := gcc

.DEFAULT_GOAL := lib

.PHONY: lib
lib:
	cd src; $(MAKE) CC=$(CC)

.PHONY: test
test:
	cd test; $(MAKE) test CC=$(CC)

.PHONY: install
install:
	cd src; $(MAKE) install

.PHONY: clean
clean:
	cd src; $(MAKE) clean
	cd test; $(MAKE) clean
