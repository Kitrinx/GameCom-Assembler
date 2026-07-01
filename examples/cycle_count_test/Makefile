TOOLS_DIR := ../..
BUILD_DIR := build
AS := $(TOOLS_DIR)/build/gamecom-as
ROMTOOL := $(TOOLS_DIR)/build/gamecom-romtool

RAW := $(BUILD_DIR)/gamecom_cycle_count_test.raw
ROM := $(BUILD_DIR)/gamecom_cycle_count_test.tgc
LST := $(BUILD_DIR)/gamecom_cycle_count_test.lst

.PHONY: all tools verify clean

all: $(ROM)

tools:
	$(MAKE) -C $(TOOLS_DIR) bins

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(RAW): cycle_count_test.asm tools | $(BUILD_DIR)
	$(AS) cycle_count_test.asm -o $@ --lst $(LST)

$(ROM): $(RAW) tools
	$(ROMTOOL) build $< -o $@ --title "CYC TEST" --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --header-offset 0x40000 --min-size 0x100000 --icon-bank 0 --icon-loc 0 --security-root both
	$(ROMTOOL) verify $@ --require-root both

verify: $(ROM)
	$(ROMTOOL) verify $(ROM) --require-root both

clean:
	rm -rf $(BUILD_DIR)
