# Game.com Hello World

This example builds from:

- `hello.asm`
- `test.chr`, copied from `reference/test.chr`
- the compiled C tools in `../../build/`

Build from the package root:

```bash
make -C examples/hello_world clean all
```

The output ROM is `examples/hello_world/build/hello_world.tgc`. The ROM tool
expands the raw assembler output to a 1 MiB power-of-two image, patches the
cartridge header checksum, and writes both compatibility security triplets. No
`data.bin`, source generator, or script helper is used.

The ROM is intended to boot through the normal BIOS menu. The entry dispatcher
returns for BIOS init/reset/close commands and starts only for the execute
command issued when the cartridge is selected. The header advertises no icon
asset (`icon bank 0`), so the BIOS draws the program-string fallback.
