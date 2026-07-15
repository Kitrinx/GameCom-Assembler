# Game.com Cycle Count Test ROM

This third ASM example builds a cartridge-side SM8521 timing probe.

It iterates over all 256 primary opcode bytes with ROM-resident executable
payloads, stores measured cycle counts or explicit skip markers in internal
RAM, and renders a 32-page report on the LCD. Each page covers one eight-byte
opcode range such as `OP 00-07` or `OP F8-FF`.

Each executable row uses an immediately paired baseline and opcode run. Both
runs restart Timer0 and begin from the same prepared register, flag, pointer,
interrupt, and sandbox-memory state. Three trials are taken; a two-of-three
match is displayed as the hexadecimal cycle count, while a row without a
majority is displayed as `??`. This replaces the previous 128-repeat modulo
fingerprint, whose raw `82`/`83` values were not human-readable cycle counts.

Executable test bytes are never copied to or fetched from internal RAM. A
real-hardware capture of the earlier version reset at progress opcode `00`,
exactly when its first measurement called a generated baseline `RET` at RAM
address `0310h`. This localizes the failure to the generated-RAM trampoline but
does not prove that internal RAM is non-executable. The BIOS normally clears,
reads, and writes this region as work RAM, and the datasheet does not explicitly
forbid instruction fetches from it. Every runnable descriptor now contains its
opcode plus an inline `RET` in cartridge ROM, eliminating both self-modifying
code and collisions with BIOS work data. Baseline and opcode paths both use the
same indirect-call form so dispatch overhead still cancels.

`RET` is calibrated through a controlled private-stack trampoline. The measured
RET cost is removed from the two CALL rows, so those rows display CALL itself
rather than CALL plus its target RET. Unsafe/non-returning primary opcodes such
as `HALT`, `STOP`, `DM`, `MOV PS0`, `CALS`, `IRET`, unbalanced stack probes,
board-connected `MOV P0-P3,#imm`, and invalid/reserved opcodes are shown as
`--`.

Opcodes `CC-CF` cannot be executed safely by a general cartridge timing ROM.
They write fixed immediates to the physical P0-P3 data latches; P3 bits 7:6
select the active cartridge slot. The previous `CF MOV P3,#FFh` probe selected
neither slot and disconnected the ROM containing its next instruction. There
is also no one immediate that preserves both slot 1 and slot 2. These four rows
therefore remain present as intentional skips instead of reporting a guessed
or inferred measurement. `C8-CB` remain measured with zero because IE0, IE1,
IR0, and IR1 are already cleared by the interrupt-disabled harness.

The hardware setup explicitly stops and clears the watchdog, selects register
bank zero, installs a private 16-bit stack, disables interrupts and DMA, and
requires `CKC[5:3]=100` so the CPU and Timer0 both use main clock divided by
two. A mismatched clock displays `CLOCK IS NOT FCK/2` instead of producing
scaled results. During measurement, the current opcode is shown on screen to
localize any remaining hardware-only failure.

The values are cartridge-observable SM8521 Timer0 counts. They are intended for
real-hardware characterization, but each row still represents one documented
safe canonical operand form rather than every possible descriptor byte or both
paths of every conditional encoding.

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

The generated `.tgc` may be loaded on Game.com hardware or in a compatible
emulator. Simulator executables and copyrighted BIOS images are intentionally
not external build dependencies. Use
`make -C examples/cycle_count_test verify` for the self-contained package
verification. Press any main button to advance report pages at runtime.
