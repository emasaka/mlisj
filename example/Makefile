TARGET := mlisj-example
LIB_DIR := ../src

.PHONY: all
all: $(TARGET)

$(TARGET): mlisj-example.c $(LIB_DIR)/libmlisj.so
	cc mlisj-example.c $(LIB_DIR)/libmlisj.so -o $(TARGET)

clean:
	rm -f $(TARGET)
