# Game.com ASM Sound Test ROMs

This folder contains assembly-source versions of the two sound test ROMs:

- `sound_phase_test.asm`
- `sound_characterization_test.asm`

The build uses only the native C tools in `../../build/`:

- `gamecom-as`
- `gamecom-romtool`

No Python generator, script helper, or HDK `data.bin` file is invoked.

## Assets

- `assets/phase_spans.bin`
- `assets/recording_spans.bin`

These are pre-rendered Game.com VRAM span graphics copied from the previous
generated ROM outputs so the on-screen phase/progress pages stay visually
compatible. The executable flow, sound register writes, wave tables, DAC sample
table, input handling, and cartridge headers are expressed in assembly.

`assets/test.chr` is kept with the project as the original font source used to
make those screen assets.

## Build

From the package root:

```bash
make -C examples/sound_tests_asm clean all
```

Outputs are written under `examples/sound_tests_asm/build/`:

- `build/gamecom_sound_phase_test.raw`
- `build/gamecom_sound_phase_test.tgc`
- `build/gamecom_sound_phase_test.lst`
- `build/gamecom_sound_characterization_test.raw`
- `build/gamecom_sound_characterization_test.tgc`
- `build/gamecom_sound_characterization_test.lst`

Both `.tgc` files are expanded to 1 MiB, header-patched, checksum-patched, and
security-triplet-patched by `gamecom-romtool`.

The ASM cartridges now use normal BIOS menu-launch entry behavior. Init and
reset return to the BIOS, close mutes sound and returns, and only the execute
command starts the test. Since no cart icon image bank is included, the ROM
headers set icon bank `0` and let the BIOS draw the program-string fallback.
