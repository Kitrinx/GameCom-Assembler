# Game.com Toolchain

Native C tools and assembly examples for building Tiger Game.com cartridge
ROMs.

## Tools

- `gamecom-as`: SM8521 assembler.
- `gamecom-link`: links `gamecom-as --obj` relocatable objects, resolves
  absolute-word fixups, can import legacy HDK syscall-style `.obj` libraries,
  and can place fixed binary assets with `--incbin PATH@ADDR`.
- `gamecom-romtool`: expands raw assembler output, writes cartridge headers,
  patches checksums/security bytes, and verifies ROMs.
- `gamecom-pack-hdk`: compatibility packer for old HDK-style projects.

Build the tools and run the assembler, linker, and ROM-tool smoke tests:

```bash
make
```

The binaries are written to `build/`.

## Tool Reference

### `gamecom-as`

Assembles SM8521/Game.com assembly into either flat bytes or a native object
file for `gamecom-link`.

```bash
build/gamecom-as SOURCE -o OUTPUT [--obj] [--lst LISTING] [--base EXPR] [--start EXPR] [--define NAME=EXPR] [--case-sensitive] [--sdk-compat]
```

Options:

- `-o`, `--output`: output file.
- `--lst LISTING`: write an address/byte/source listing.
- `--base EXPR`: choose the first address represented by the flat output. If
  omitted, output starts at the first emitted address. Use `org` in the source
  to set program addresses.
- `--start EXPR`, `--org EXPR`: set the initial assembly PC before reading
  source. This is useful for fixed-placement legacy modules that do not carry
  their own `org`.
- `--obj`, `--object`: write a native `GCO1` object instead of flat bytes.
- `--define NAME=EXPR`: define or override a symbol before assembly.
- `--case-sensitive`: make symbols case-sensitive. The default is
  case-insensitive, matching common HDK-era Game.com source.
- `--sdk-compat`, `--asm8521-compat`: enable official SDK/ASM8521 dialect
  compatibility such as bare no-colon label lines, trailing descriptive text
  after `equ` values, and the SDK's left-to-right expression evaluation. This
  is intentionally opt-in.

Assembly syntax highlights:

- Labels with or without `:`, `equ`/`set`, `org`/`base`, `include`, `incbin`,
  `db`/`defb`/`defm`/`dm`, `dw`/`defw`, `ds`/`defs`, `fill`, and `align`.
- `program` and `data` section markers from official-style source.
- SM8521 SFR aliases such as `LCC`, `DMC`, `TM0C`, `SGDA`, and interrupt
  vectors.
- Official-style numeric forms such as `0ffh` and `00000011b`, plus `$1234`
  and `0x1234`.
- WLA-style operand size hints: `.b` forces 8-bit direct/byte forms and `.w`
  forces 16-bit absolute/word forms for ambiguous operands.
- Square brackets are accepted as memory parentheses, so `[rr4]` and `(rr4)`
  assemble the same way.
- Official SDK register aliases from `SM85.TYP` are provided, including
  lowercase forms under `--case-sensitive`.

Object mode notes:

- Native objects preserve assembled addresses and exports.
- `global` and `public` export named symbols. If no globals are present, labels
  are exported for convenience.
- Unresolved simple absolute references in `call`, `jmp`, conditional `jmp`,
  and `dw` become linker fixups.
- Relative branches such as `br` and `dbnz` must resolve inside the same
  object.

Flat-output example:

```bash
build/gamecom-as examples/hello_world/hello.asm \
  -o examples/hello_world/build/hello_world.raw \
  --lst examples/hello_world/build/hello_world.lst
```

Object-output example:

```bash
build/gamecom-as main.asm --obj -o build/main.gco
build/gamecom-as lib.asm --obj -o build/lib.gco
```

### `gamecom-link`

Links native `GCO1` objects from `gamecom-as --obj`, resolves absolute-word
fixups, can import the official syscall-style HDK `.obj` shape, and can place
fixed binary assets.

```bash
build/gamecom-link INPUT... -o OUTPUT [--base ADDR] [--map MAP] [--incbin PATH@ADDR] [--symbols-only OBJ]
```

Options:

- `-o`, `--output`: linked flat byte output.
- `--base ADDR`: first address represented by the output file. If omitted, the
  lowest linked address is used. The base cannot be after the first linked
  byte.
- `--map MAP`: write linked chunks, symbols, and fixup locations.
- `--incbin PATH@ADDR`: place a raw binary asset at an absolute address after
  object placement. This is useful for speech/audio blobs or other fixed banks.
- `--symbols-only OBJ`: import an object's exported symbols without placing its
  bytes. This is useful for banked legacy projects where each bank window is
  linked separately but cross-bank calls still need address resolution.

Linking model:

- Native objects are fixed-address objects. Place code with `org` in assembly.
- Native object exports are resolved case-insensitively by default, matching
  the assembler.
- Imported legacy HDK `.obj` support is intentionally narrow: it supports the
  official syscall-library object shape used by `syscall.obj`.
- Legacy objects are packed into the next free address after native object
  code before fixed `--incbin` assets are applied. This leaves room for assets
  at addresses such as `6000h`.
- Output is flat bytes suitable for `gamecom-romtool build`.

Native object/link flow:

```bash
build/gamecom-as main.asm --obj -o build/main.gco
build/gamecom-as lib.asm --obj -o build/lib.gco
build/gamecom-link build/main.gco build/lib.gco -o build/program.raw --base 4000h
build/gamecom-romtool build build/program.raw -o build/program.tgc
```

Legacy syscall object plus fixed asset flow:

```bash
build/gamecom-link build/main.gco syscall.obj --incbin sound.bin@6000h -o build/program.raw --base 4000h
```

### `gamecom-romtool`

Builds and verifies normal Game.com cartridge ROM images. Use this for new
ROMs after assembling or linking the program bytes.

Build mode:

```bash
build/gamecom-romtool build INPUT -o OUTPUT [options]
```

Build options:

- `-o`, `--output`: output cartridge image.
- `--header-offset N`: Game.com header location. Default: `0x40000`.
- `--raw-offset N`: where to place the input bytes. Default: same as
  `--header-offset`.
- `--min-size N`: minimum output size before power-of-two expansion. Default:
  `0x100000` bytes.
- `--fill BYTE`: fill byte for unused ROM space. Default: `0xff`.
- `--entry-bank BYTE`: entry bank in the cartridge header. Default: `0x20`.
- `--entry-addr WORD`: entry address in the cartridge header. Default:
  `0x4020`.
- `--flags BYTE`: header flags byte. Default: `0x03`.
- `--icon-bank BYTE`: BIOS cart icon bank. Default: `0`.
- `--icon-loc WORD`: BIOS cart icon location. Default: `0`.
- `--title TEXT`: title field, up to 9 ASCII bytes. Default: `GAMECOM`.
- `--program-id WORD`: program ID used to derive the header checksum. Default:
  `0x1a18`.
- `--security-root absolute|header|both`: where to patch the security triplet.
  Default: `both`.
- `--allow-security-overwrite`: allow a security byte to modify raw payload
  bytes when unavoidable.
- `--quiet`: suppress summary output.

Build behavior:

- Expands the output to a nonzero power of two.
- Writes the Game.com header, signature, entry point, title, checksum, icon
  fields, and program ID.
- Patches the security triplet selected by the checksum low nibble.
- Defaults to a 1 MiB ROM with both known security roots valid.

Verify mode:

```bash
build/gamecom-romtool verify INPUT [--header-offset N] [--require-root any|absolute|header|both] [--quiet]
```

Verify checks that the ROM size is a power of two, locates the Game.com header,
checks the program-ID-derived checksum, and verifies the requested security
root. `--require-root both` is the usual final ROM check for these examples.

Example:

```bash
build/gamecom-romtool build build/program.raw -o build/program.tgc \
  --title HELLOWRLD \
  --program-id 0x1a18 \
  --security-root both

build/gamecom-romtool verify build/program.tgc --require-root both
```

### `gamecom-pack-hdk`

Compatibility packer for old HDK-style projects that separate assembled code
from binary graphics banks. New projects should usually prefer
`gamecom-as`/`gamecom-link` plus `gamecom-romtool`; keep this tool for source
ports and byte comparisons against HDK layouts.

```bash
build/gamecom-pack-hdk INPUT -o OUTPUT [--hdk-data PATH] [--gfx-dir DIR] [--incbin PATH@OFFSET] [--size N] [--program-offset N] [--fill BYTE]
```

Options:

- `-o`, `--output`: output packed image.
- `--hdk-data PATH`: optional old HDK filler/template data. Use only when a
  byte comparison to an old HDK output requires it.
- `--gfx-dir DIR`: directory containing `bankNN.BIN` files.
- `--incbin PATH@OFFSET`: overlay an additional binary at an image-relative
  offset. Repeat this option for SDK-style projects that place separate code
  or data ranges at fixed ROM offsets.
- `--size N`: force output size. If omitted, the tool chooses 1 MiB or 2 MiB
  depending on program/template/`--incbin` size.
- `--program-offset N`: offset where assembled program bytes are overlaid.
  Default: `0x40000`.
- `--fill BYTE`: fill byte for unused space. Default: `0xff`.

Graphics placement:

- `bankNN.BIN` files are detected by decimal digits in the filename.
- Each bank is copied to `NN * 0x4000`.
- At most `0x4000` bytes from each bank file are copied.
- `--incbin` overlays are applied after the main program/template and graphics
  bank copies, so explicit placements can intentionally replace earlier bytes.

Example:

```bash
build/gamecom-pack-hdk tetris.raw \
  --gfx-dir examples/tetris/assembler/source/gfx \
  --fill 0xff \
  -o build/tetris_packed.bin
```

SDK-style fixed placement example:

```bash
build/gamecom-pack-hdk bank20.raw -o build/game_packed.bin \
  --program-offset 0x40000 \
  --incbin bank23.raw@0x46000 \
  --incbin bank25.raw@0x4a000 \
  --gfx-dir build/art_banks
```

## Examples

Build all examples:

```bash
make examples
```

The examples are:

- `examples/hello_world`
- `examples/sound_tests_asm`
- `examples/cycle_count_test`
- `examples/tetris`

Each example emits `.raw`, `.lst`, and signed 1 MiB `.tgc` files under its
local build directory. Tetris also emits an intermediate packed image because
it has separate binary graphics banks.

The example cartridges use normal BIOS-menu entry behavior. Init/reset/close
commands return to the BIOS, and the program starts only when the BIOS issues
the execute command after the cartridge is selected. They advertise no cart
icon (`icon bank 0`) until a real icon image bank is supplied.
Tetris is the exception: it carries its original icon graphics in bank `0x24`.

## Common Commands

```bash
make              # build tools and run tool tests
make examples     # build tools and all example ROMs
make verify-examples
make clean
```

## Layout

```text
src/              C source for the tools
tests/fixtures/   assembler, linker, and ROM-tool smoke fixtures
examples/         assembly-source cartridge examples
build/            generated tool binaries and test artifacts
```

## License

The C build tools in `src/` are distributed under the GNU General Public
License version 3. See `LICENSE` in this folder.

Example programs, imported source, and graphical/audio assets may have their
own license files or attribution notes in their example directories. For
example, `examples/tetris/` preserves its upstream MIT license and README.
