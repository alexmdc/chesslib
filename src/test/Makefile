CC=clang
CFLAGS=-g -Wall -std=c89 -pedantic
LDFLAGS=-lcunit
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS=$(SRCS:%.c=$(BUILD_DIR)/%.d)
BUILD_DIR?=build
LIB=../$(BUILD_DIR)/libchesslib.a
EXE=$(BUILD_DIR)/chess-test

all: $(EXE)

$(EXE): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(EXE) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/%.d | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.d: %.c | $(BUILD_DIR)
	$(CC) -MM $< -MT $(@:%.d=%.o) -MF $@

ifneq ($(MAKECMDGOALS), clean)
    -include $(DEPS)
endif

clean:
	rm -Rf $(BUILD_DIR)

.PHONY: clean
