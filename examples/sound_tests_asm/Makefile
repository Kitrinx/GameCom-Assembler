TOOLS_DIR := ../..
BUILD_DIR := build
AS := $(TOOLS_DIR)/build/gamecom-as
ROMTOOL := $(TOOLS_DIR)/build/gamecom-romtool

PHASE_RAW := $(BUILD_DIR)/gamecom_sound_phase_test.raw
PHASE_ROM := $(BUILD_DIR)/gamecom_sound_phase_test.tgc
PHASE_LST := $(BUILD_DIR)/gamecom_sound_phase_test.lst
REC_RAW := $(BUILD_DIR)/gamecom_sound_characterization_test.raw
REC_ROM := $(BUILD_DIR)/gamecom_sound_characterization_test.tgc
REC_LST := $(BUILD_DIR)/gamecom_sound_characterization_test.lst

.PHONY: all tools verify clean

all: $(PHASE_ROM) $(REC_ROM)

tools:
	$(MAKE) -C $(TOOLS_DIR) bins

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(PHASE_RAW): sound_phase_test.asm sound_defs.inc sound_routines.inc sound_data_common.inc assets/phase_spans.bin tools | $(BUILD_DIR)
	$(AS) sound_phase_test.asm -o $@ --lst $(PHASE_LST)

$(REC_RAW): sound_characterization_test.asm sound_defs.inc sound_routines.inc sound_recording_routines.inc sound_data_common.inc sound_recording_data.inc assets/recording_spans.bin tools | $(BUILD_DIR)
	$(AS) sound_characterization_test.asm -o $@ --lst $(REC_LST)

$(PHASE_ROM): $(PHASE_RAW) tools
	$(ROMTOOL) build $< -o $@ --title "SND PHASE" --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --header-offset 0x40000 --min-size 0x100000 --icon-bank 0 --icon-loc 0 --security-root both
	$(ROMTOOL) verify $@ --require-root both

$(REC_ROM): $(REC_RAW) tools
	$(ROMTOOL) build $< -o $@ --title "SND REC" --program-id 0x1a18 --entry-bank 0x20 --entry-addr 0x4020 --header-offset 0x40000 --min-size 0x100000 --icon-bank 0 --icon-loc 0 --security-root both
	$(ROMTOOL) verify $@ --require-root both

verify: $(PHASE_ROM) $(REC_ROM)
	$(ROMTOOL) verify $(PHASE_ROM) --require-root both
	$(ROMTOOL) verify $(REC_ROM) --require-root both

clean:
	rm -rf $(BUILD_DIR)
