CC ?= cc
CFLAGS ?= -std=c99 -O2 -Wall -Wextra

BUILD_DIR := build
BINS := $(BUILD_DIR)/gamecom-as $(BUILD_DIR)/gamecom-pack-hdk $(BUILD_DIR)/gamecom-romtool
AS_BIN := $(BUILD_DIR)/gamecom-as
ROMTOOL := $(BUILD_DIR)/gamecom-romtool

EXAMPLE_DIRS := \
	examples/hello_world \
	examples/sound_tests_asm \
	examples/cycle_count_test \
	examples/tetris

.PHONY: all tools bins test coverage examples verify-examples clean clean-examples

all: tools

tools: bins test

bins: $(BINS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/gamecom-as: src/gamecom_as.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/gamecom-pack-hdk: src/gamecom_pack_hdk.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/gamecom-romtool: src/gamecom_romtool.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

coverage: bins
	$(AS_BIN) tests/fixtures/all_opcodes.asm -o $(BUILD_DIR)/all_opcodes.bin --lst $(BUILD_DIR)/all_opcodes.lst
	xxd -p -c 1000000 $(BUILD_DIR)/all_opcodes.bin > $(BUILD_DIR)/all_opcodes.hex
	diff -u tests/fixtures/all_opcodes.expected.hex $(BUILD_DIR)/all_opcodes.hex

test: coverage
	$(AS_BIN) tests/fixtures/wla_style_ambiguity.asm -o $(BUILD_DIR)/wla_style_ambiguity.bin --lst $(BUILD_DIR)/wla_style_ambiguity.lst
	xxd -p -c 1000000 $(BUILD_DIR)/wla_style_ambiguity.bin > $(BUILD_DIR)/wla_style_ambiguity.hex
	diff -u tests/fixtures/wla_style_ambiguity.expected.hex $(BUILD_DIR)/wla_style_ambiguity.hex
	$(ROMTOOL) build $(BUILD_DIR)/all_opcodes.bin -o $(BUILD_DIR)/signed_smoke.bin --title SMOKETEST --quiet
	$(ROMTOOL) verify $(BUILD_DIR)/signed_smoke.bin --require-root both --quiet

examples: tools
	@for dir in $(EXAMPLE_DIRS); do \
		$(MAKE) -C $$dir all || exit $$?; \
	done

verify-examples: examples
	@for dir in $(EXAMPLE_DIRS); do \
		$(MAKE) -C $$dir verify || exit $$?; \
	done

clean-examples:
	@for dir in $(EXAMPLE_DIRS); do \
		$(MAKE) -C $$dir clean || exit $$?; \
	done

clean: clean-examples
	rm -rf $(BUILD_DIR)
