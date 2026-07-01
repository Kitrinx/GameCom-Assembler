# Game.com Cycle Count Test ROM

This third ASM example builds a cartridge-side SM8521 timing probe.

It calibrates Timer0 overhead, runs all 256 primary opcode bytes through a
tiny internal-RAM stub harness, stores the measured tick deltas in internal
RAM, and renders a 32-page report on the LCD. Each page covers one eight-byte
opcode range such as `OP 00-07` or `OP F8-FF`.

Normal executable rows show the calibrated 128-iteration delta byte. Long
operators such as `MULT` and `DIV D,#` use a one-iteration baseline to avoid
8-bit Timer0 wrap. Unsafe/non-local primary opcodes such as `HALT`, `STOP`,
`DM`, `MOV PS0`, `CALS`, `IRET`, stack-only probes, invalid/reserved opcodes,
and the currently unsafe register-pair `DIV` probe are shown as `--`.

The displayed values are cartridge-observable Timer0 tick deltas for the
current core/test environment. They are useful for comparing primary-opcode
timing changes, but they should not be treated as final silicon cycle truth
until the timer cadence audit is fully closed. Descriptor sub-opcode spaces are
covered by canonical safe descriptor forms, not by every possible descriptor
byte.

Build from the package root:

```bash
make -C examples/cycle_count_test clean all
```

Outputs are written under `examples/cycle_count_test/build/`:

- `build/gamecom_cycle_count_test.raw`
- `build/gamecom_cycle_count_test.tgc`
- `build/gamecom_cycle_count_test.lst`

The build path uses only the native C `gamecom-as` and `gamecom-romtool`
binaries.

The cartridge is BIOS-menu friendly: init/reset/close entry commands return
without starting the measurement harness, and only the BIOS execute command
launches the test. It advertises no icon bank, so the BIOS uses the text
fallback on the cartridge menu.

Optional local live-sim smoke example, when this folder sits next to
`GameCom_MiSTer`:

```bash
../GameCom_MiSTer/agents/tools/live_verilator_sdl_sim/build/gamecom_live \
  --bios "../GameCom_MiSTer/reference/ROMs/Game.com External BIOS (1997) (71-516) [!].bin" \
  --cart examples/cycle_count_test/build/gamecom_cycle_count_test.tgc \
  --no-window --fast --auto-power --stop-disable --max-cycles 220000000 \
  --dump-frame examples/cycle_count_test/build/cycle_count_page0.ppm \
  --debug-summary
```

Press any main button to advance pages. In the simulator, use repeated
`--script-press START,LEN,a` windows after the first page is drawn to verify
pagination.
