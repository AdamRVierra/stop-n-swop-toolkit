BUILD_DIR := build

#N64_ROM_HEADER_SRC := /c/users/adam/desktop/libdragon/boot/bin/ipl3_dev.z64
N64_ROM_HEADER := ipl3_dev.z64

N64_ROM_LAYOUT := dev
ROM_TITLE := Stop 'N' Swop
include $(N64_INST)/include/n64.mk

src := \
  stopnswop/stopnswop.c \
  stopnswop_example.c

assets_png  := $(wildcard assets/*.png)
assets_conv := $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))

MKSPRITE_FLAGS ?=

all: stopnswop.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -f RGBA16 -o "$(dir $@)" "$<"

$(BUILD_DIR)/stopnswop.dfs: $(assets_conv)
$(BUILD_DIR)/stopnswop.elf: $(src:%.c=$(BUILD_DIR)/%.o)

$(BUILD_DIR):
	@mkdir -p $@

#$(N64_ROM_HEADER):
#	@echo "    [HDR] Copying $(N64_ROM_HEADER_SRC) -> $(N64_ROM_HEADER)"
#	@cp "$(N64_ROM_HEADER_SRC)" "$(N64_ROM_HEADER)"

stopnswop.z64: $(BUILD_DIR)/stopnswop.elf $(BUILD_DIR)/stopnswop.dfs $(N64_ROM_HEADER)
	$(N64_INST)/bin/n64sym "$(BUILD_DIR)/stopnswop.elf" "$(BUILD_DIR)/stopnswop.elf.sym"
	cp "$(BUILD_DIR)/stopnswop.elf" "$(BUILD_DIR)/stopnswop.elf.stripped"
	$(N64_INST)/bin/mips64-elf-strip -s "$(BUILD_DIR)/stopnswop.elf.stripped"
	$(N64_INST)/bin/n64elfcompress -o "$(BUILD_DIR)/" -c 1 "$(BUILD_DIR)/stopnswop.elf.stripped"
	$(N64_INST)/bin/n64tool -o "stopnswop.z64" -h "$(N64_ROM_HEADER)" -t "$(ROM_TITLE)" -T --align 256 "$(BUILD_DIR)/stopnswop.elf.stripped" --align 8 "$(BUILD_DIR)/stopnswop.elf.sym" --align 16 "$(BUILD_DIR)/stopnswop.dfs"
	@if [ ! -z "$(ED64ROMCONFIG)" ]; then $(N64_INST)/bin/ed64romconfig "stopnswop.z64"; fi

clean:
	rm -rf "$(BUILD_DIR)" filesystem/ stopnswop.z64 

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
