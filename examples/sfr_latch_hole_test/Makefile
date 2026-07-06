TOOLS_DIR := ../..
BUILD_DIR := build
AS := $(TOOLS_DIR)/build/gamecom-as
ROMTOOL := $(TOOLS_DIR)/build/gamecom-romtool

.PHONY: all tools verify clean

all: $(BUILD_DIR)/sfr_latch_hole_test.tgc

tools:
	$(MAKE) -C $(TOOLS_DIR) bins

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/sfr_latch_hole_test.raw: sfr_latch_hole_test.asm ../common/gc_test_defs.inc ../common/gc_test_common.inc tools | $(BUILD_DIR)
	$(AS) sfr_latch_hole_test.asm -o $@ --lst $(BUILD_DIR)/sfr_latch_hole_test.lst

$(BUILD_DIR)/sfr_latch_hole_test.tgc: $(BUILD_DIR)/sfr_latch_hole_test.raw tools
	$(ROMTOOL) build $< -o $@ --title SFRTEST --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --header-offset 0x40000 --min-size 0x100000 --icon-bank 0 --icon-loc 0 --security-root both
	$(ROMTOOL) verify $@ --require-root both

verify: $(BUILD_DIR)/sfr_latch_hole_test.tgc
	$(ROMTOOL) verify $< --require-root both

clean:
	rm -rf $(BUILD_DIR)
