.DEFAULT_GOAL := lib

.PHONY: lib
lib:
	cd src; $(MAKE)

.PHONY: test
test:
	cd test; $(MAKE) test

.PHONY: clean
clean:
	cd src; $(MAKE) clean
	cd test; $(MAKE) clean
