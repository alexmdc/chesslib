BUILD_DIR=build
LIB=$(BUILD_DIR)/libchesslib.a

all: lib shell test-bin

lib:
	$(MAKE) -C src $(MAKEOPTS) BUILD_DIR=../$(BUILD_DIR)

shell: lib
	$(MAKE) -C src/shell $(MAKEOPTS) BUILD_DIR=../../$(BUILD_DIR)/shell LIB=../../$(LIB)

test-bin: lib
	$(MAKE) -C src/test $(MAKEOPTS) BUILD_DIR=../../$(BUILD_DIR)/test LIB=../../$(LIB)

test: test-bin
	$(BUILD_DIR)/test/chess-test

clean:
	rm -Rf $(BUILD_DIR)

.PHONY: clean lib shell test test-bin
