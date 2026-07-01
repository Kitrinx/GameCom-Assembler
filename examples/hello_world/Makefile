TOOLS_DIR := ../..
BUILD_DIR := build
AS := $(TOOLS_DIR)/build/gamecom-as
ROMTOOL := $(TOOLS_DIR)/build/gamecom-romtool

.PHONY: all tools verify clean

all: $(BUILD_DIR)/hello_world.tgc

tools:
	$(MAKE) -C $(TOOLS_DIR) bins

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/hello_world.raw: hello.asm test.chr tools | $(BUILD_DIR)
	$(AS) hello.asm -o $@ --lst $(BUILD_DIR)/hello_world.lst

$(BUILD_DIR)/hello_world.tgc: $(BUILD_DIR)/hello_world.raw tools
	$(ROMTOOL) build $< -o $@ --title HELLOWRLD --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --header-offset 0x40000 --min-size 0x100000 --icon-bank 0 --icon-loc 0 --security-root both
	$(ROMTOOL) verify $@ --require-root both

verify: $(BUILD_DIR)/hello_world.tgc
	$(ROMTOOL) verify $< --require-root both

clean:
	rm -rf $(BUILD_DIR)
