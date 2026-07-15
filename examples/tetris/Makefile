TOOLS_DIR := ../..
TOOLS_BUILD_DIR := $(abspath $(TOOLS_DIR)/build)
ASM_DIR := assembler
BUILD_DIR := $(ASM_DIR)/build
AS := $(TOOLS_BUILD_DIR)/gamecom-as
PACK_HDK := $(TOOLS_BUILD_DIR)/gamecom-pack-hdk
ROMTOOL := $(TOOLS_BUILD_DIR)/gamecom-romtool

RAW := $(BUILD_DIR)/tetris.raw
PACKED := $(BUILD_DIR)/tetris_packed.bin
ROM := $(BUILD_DIR)/tetris.tgc
LST := $(BUILD_DIR)/tetris.lst

ASM_SOURCES := $(wildcard $(ASM_DIR)/source/*.ASM) $(wildcard $(ASM_DIR)/source/*.asm) $(wildcard $(ASM_DIR)/source/*.INC)
GFX_BANKS := $(wildcard $(ASM_DIR)/source/gfx/*.BIN)

.PHONY: all tools verify clean

all: $(ROM)

tools:
	$(MAKE) -C $(TOOLS_DIR) bins

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(RAW): $(ASM_SOURCES) tools | $(BUILD_DIR)
	cd $(ASM_DIR) && "$(AS)" source/MAIN.ASM -o build/tetris.raw --lst build/tetris.lst --sdk-compat --case-sensitive

$(PACKED): $(RAW) $(GFX_BANKS) tools
	cd $(ASM_DIR) && "$(PACK_HDK)" build/tetris.raw -o build/tetris_packed.bin --gfx-dir source/gfx --size 0x100000 --fill 0xff

$(ROM): $(PACKED) tools
	cd $(ASM_DIR) && "$(ROMTOOL)" build build/tetris_packed.bin -o build/tetris.tgc --raw-offset 0 --header-offset 0x40000 --min-size 0x100000 --title TetrisTGM --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --flags 0x03 --icon-bank 0x24 --icon-loc 0x00a0 --security-root both --allow-security-overwrite
	cd $(ASM_DIR) && "$(ROMTOOL)" verify build/tetris.tgc --require-root both

verify: $(ROM)
	cd $(ASM_DIR) && "$(ROMTOOL)" verify build/tetris.tgc --require-root both

clean:
	rm -rf $(BUILD_DIR)
