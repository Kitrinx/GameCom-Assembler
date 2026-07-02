# Game.com Toolchain

Native C tools and assembly examples for building Tiger Game.com cartridge
ROMs.

## Tools

- `gamecom-as`: SM8521 assembler.
- `gamecom-romtool`: expands raw assembler output, writes cartridge headers,
  patches checksums/security bytes, and verifies ROMs.
- `gamecom-pack-hdk`: compatibility packer for old HDK-style projects.

Build the tools and run the assembler/ROM-tool smoke tests:

```bash
make
```

The binaries are written to `build/`.

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
tests/fixtures/   assembler and ROM-tool smoke fixtures
examples/         assembly-source cartridge examples
build/            generated tool binaries and test artifacts
```
