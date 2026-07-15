# Game.com Tetris

This example builds Tpot's MIT-licensed Game.com Tetris source with the native
C tools in `../../build/`.

## Origin

The original project is `Tetris_GameCOM`, a Game.com homebrew Tetris project
based on Tetris The Grandmaster.

- Upstream source: `https://github.com/Tpot-SSL/Tetris_GameCOM`
- Original author / copyright holder: Tpot
- Git commit author in the copied upstream history: Derrek Harbold
  `<thebananaofstone@gmail.com>`
- Copied revision: `1b55b8c6e5d11dc1febfc40e2ac42ffcfe998291`
  (`Improved input handling`, 2018-03-13)
- License: MIT, preserved in `LICENSE`
- Upstream README snapshot: `UPSTREAM_README.md`

This folder is a toolchain example copy of that upstream project. The build
layout and compatibility fixes are local to this package; the game source,
graphics banks, license, and original README are retained so the attribution is
visible next to the buildable example.

The build inputs are:

- `assembler/source/MAIN.ASM` and its included ASM/INC files
- `assembler/source/gfx/bank36.BIN` through `bank41.BIN`
- `gamecom-as`
- `gamecom-pack-hdk`
- `gamecom-romtool`

No HDK `data.bin`, source generator, or script helper is used. The pack step
places the assembled program at `0x40000`, fills unused image bytes with
`0xff`, and copies each `bankNN.BIN` graphics asset to `NN * 0x4000`. The ROM
tool then patches the header checksum and both compatibility security triplets.

Build from the package root:

```bash
make -C examples/tetris clean all
```

Outputs are written under `examples/tetris/assembler/build/`:

- `build/tetris.raw`
- `build/tetris_packed.bin`
- `build/tetris.tgc`
- `build/tetris.lst`

The header uses the original Tetris metadata: entry `20:4020`, title
`TetrisTGM`, program ID `0x1a18`, icon bank `0x24`, and icon location
`0x00a0`. The entry dispatcher is BIOS-menu friendly: init/reset/close return
to the BIOS and execute starts the game.

The copied source has one compatibility fix in `FUNCS.ASM`: `WaitForVInt`
restores `IE1=0x50` and executes `ei` before polling the BIOS-maintained
`DMG_timer` at `0x0112`. BIOS drawing/syscall paths can temporarily clear
`IE1`, so restoring it only once at entry can let the game fade to black and
then stall on a later wait.

Compared with the earlier checked local raw artifact, this assembler emits one
intentional byte difference in `Wait10Seconds`: `br nz,waitloop` branches back
to the local loop label instead of being captured by the later `waitLoop`
input-scan label.

The generated `.tgc` may be loaded on Game.com hardware or in a compatible
emulator. Simulator executables and copyrighted BIOS images are intentionally
not external build dependencies. Use `make -C examples/tetris verify` for the
self-contained package verification.
