CC ?= cc
CFLAGS ?= -std=c99 -O2 -Wall -Wextra

BUILD_DIR := build
BINS := $(BUILD_DIR)/gamecom-as $(BUILD_DIR)/gamecom-link $(BUILD_DIR)/gamecom-pack-hdk $(BUILD_DIR)/gamecom-romtool
AS_BIN := $(BUILD_DIR)/gamecom-as
LINK_BIN := $(BUILD_DIR)/gamecom-link
PACK_BIN := $(BUILD_DIR)/gamecom-pack-hdk
ROMTOOL := $(BUILD_DIR)/gamecom-romtool

EXAMPLE_DIRS := \
	examples/hello_world \
	examples/sound_tests_asm \
	examples/cycle_count_test \
	examples/tetris \
	examples/timer_irq_boundary_test \
	examples/dma_lcdc_visual_test \
	examples/sfr_latch_hole_test

.PHONY: all tools bins test coverage examples verify-examples clean clean-examples

all: tools

tools: bins test

bins: $(BINS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/gamecom-as: src/gamecom_as.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/gamecom-link: src/gamecom_link.c | $(BUILD_DIR)
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
	$(AS_BIN) tests/fixtures/sdk_compat.asm -o $(BUILD_DIR)/sdk_compat.bin --lst $(BUILD_DIR)/sdk_compat.lst --start 4000h --sdk-compat --case-sensitive
	xxd -p -c 1000000 $(BUILD_DIR)/sdk_compat.bin > $(BUILD_DIR)/sdk_compat.hex
	diff -u tests/fixtures/sdk_compat.expected.hex $(BUILD_DIR)/sdk_compat.hex
	$(AS_BIN) tests/fixtures/link_main.asm --obj -o $(BUILD_DIR)/link_main.gco --lst $(BUILD_DIR)/link_main.lst
	$(AS_BIN) tests/fixtures/link_lib.asm --obj -o $(BUILD_DIR)/link_lib.gco --lst $(BUILD_DIR)/link_lib.lst
	$(LINK_BIN) $(BUILD_DIR)/link_main.gco $(BUILD_DIR)/link_lib.gco -o $(BUILD_DIR)/link_test.bin --base 4000h --map $(BUILD_DIR)/link_test.map
	xxd -p -c 1000000 $(BUILD_DIR)/link_test.bin > $(BUILD_DIR)/link_test.hex
	diff -u tests/fixtures/link_test.expected.hex $(BUILD_DIR)/link_test.hex
	$(PACK_BIN) tests/fixtures/pack_base.bin -o $(BUILD_DIR)/pack_test.bin --size 0x20 --program-offset 0x10 --fill 0xee --incbin tests/fixtures/pack_overlay.bin@0x04
	xxd -p -c 1000000 $(BUILD_DIR)/pack_test.bin > $(BUILD_DIR)/pack_test.hex
	diff -u tests/fixtures/pack_test.expected.hex $(BUILD_DIR)/pack_test.hex
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
