CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR := build

.PHONY: all test java c clean

all: java c

test: all
	$(BUILD_DIR)/list_test
	@java -cp $(BUILD_DIR)/java Hello | grep -Fx "Hello, world!"

java: $(BUILD_DIR)/java/Hello.class

c: $(BUILD_DIR)/list_test

$(BUILD_DIR)/java/Hello.class: chapter01/java/Hello.java
	@mkdir -p $(BUILD_DIR)/java
	javac -d $(BUILD_DIR)/java $<

$(BUILD_DIR)/list_test: chapter01/c/doubly_linked_list.c chapter01/c/doubly_linked_list.h chapter01/c/test_doubly_linked_list.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) chapter01/c/doubly_linked_list.c chapter01/c/test_doubly_linked_list.c -o $@

clean:
	rm -rf $(BUILD_DIR)

