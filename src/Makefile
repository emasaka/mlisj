CC := gcc
CFLAGS := -O3 -W -Wall -fPIC
TARGET_LIB := libmlisj.so
OBJS := mlisj.o mempool.o symbol.o variable.o functable.o lispenv.o reader.o eval.o func-simple.o func-skk-gadgets.o util.o
LINKER_VERSION_SCRIPT := libmlisj.map
LDFLAGS := --shared -Wl,--version-script=$(LINKER_VERSION_SCRIPT)

.DEFAULT_GOAL := all

.PHONY: all clean

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS) $(LINKER_VERSION_SCRIPT)
	$(CC) $(LDFLAGS) -o $(TARGET_LIB) $(OBJS)

# Object modules

LISPENV_H_AND_DEPS := lispenv.h lispobject.h mempool.h symbol.h variable.h functable.h
LISPENV_O_AND_DEPS :=  lispenv.o mempool.o symbol.o variable.o functable.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

mlisj.o: mlisj.c mlisj.h lispobject.h lispenv.h reader.h eval.h func-simple.h func-skk-gadgets.h

mempool.o: mempool.c lispobject.h mempool.h

symbol.o: symbol.c lispobject.h mempool.h symbol.h

variable.o: variable.c lispobject.h mempool.h symbol.h variable.h

functable.o: variable.c lispobject.h mempool.h symbol.h functable.h

lispenv.o: lispenv.c $(LISPENV_H_AND_DEPS)

reader.o: reader.c util.h $(LISPENV_H_AND_DEPS)

eval.o: eval.c  eval.h reader.h func-helper.h $(LISPENV_H_AND_DEPS)

func-simple.o: func-simple.c eval.h reader.h util.h func-helper.h $(LISPENV_H_AND_DEPS)

func-skk-gadgets.o: func-skk-gadgets.c eval.h reader.h util.h func-helper.h util.h $(LISPENV_H_AND_DEPS)

util.o: util.c util.h

# other

.PHONY: clean
clean:
	rm -f $(TARGET_LIB) $(OBJS)
