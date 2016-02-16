# The default target is:
all: lib shell test-bin

# Put build files in:
BUILD_DIR=build

##
# Debug builds.
#
DEBUG_DIR=$(BUILD_DIR)/debug
DEBUG_LIB=$(DEBUG_DIR)/libchesslib.a
DEBUG_FLAGS=-g

lib:
	$(MAKE) -C src $(MAKEOPTS) BUILD_DIR=../$(DEBUG_DIR) EXTRA_CFLAGS=$(DEBUG_FLAGS)

shell: lib
	$(MAKE) -C src/shell $(MAKEOPTS) BUILD_DIR=../../$(DEBUG_DIR)/shell EXTRA_CFLAGS=$(DEBUG_FLAGS) LIB=../../$(DEBUG_LIB)

test-bin: lib
	$(MAKE) -C src/test $(MAKEOPTS) BUILD_DIR=../../$(DEBUG_DIR)/test EXTRA_CFLAGS=$(DEBUG_FLAGS) LIB=../../$(DEBUG_LIB)

test: test-bin
	$(DEBUG_DIR)/test/chess-test

.PHONY: lib shell test test-bin

##
# Code coverage.
#
COVERAGE_DIR=$(BUILD_DIR)/coverage
COVERAGE_LIB=$(COVERAGE_DIR)/libchesslib.a
COVERAGE_FLAGS="-fprofile-arcs -ftest-coverage -DNDEBUG"
COVERAGE_INFO=$(COVERAGE_DIR)/test.info
COVERAGE_HTML=$(COVERAGE_DIR)/html

coverage-lib:
	$(MAKE) -C src $(MAKEOPTS) CC=gcc BUILD_DIR=../$(COVERAGE_DIR) EXTRA_CFLAGS=$(COVERAGE_FLAGS)

coverage-test: coverage-lib
	$(MAKE) -C src/test $(MAKEOPTS) CC=gcc BUILD_DIR=../../$(COVERAGE_DIR)/test EXTRA_CFLAGS=$(COVERAGE_FLAGS) LIB=../../$(COVERAGE_LIB)

$(COVERAGE_INFO): $(COVERAGE_DIR)/test/chess-test
	lcov --zerocounters --directory $(COVERAGE_DIR) --no-recursion
	$(COVERAGE_DIR)/test/chess-test
	lcov --capture --directory $(COVERAGE_DIR) --no-recursion --base-directory src --output-file $(COVERAGE_INFO)

$(COVERAGE_HTML)/index.html: $(COVERAGE_INFO)
	genhtml $(COVERAGE_INFO) --output-directory $(COVERAGE_HTML)

coverage: coverage-test $(COVERAGE_HTML)/index.html
	-open file://$(PWD)/$(COVERAGE_HTML)/index.html

.PHONY: coverage coverage-lib coverage-test

##
# Cleaning up.
#
clean:
	rm -Rf $(BUILD_DIR)

.PHONY: clean
