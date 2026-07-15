# Game.com Edge Grid Test

This cartridge draws a native 200x160 LCD alignment pattern:

- black outer box at the physical pixels `x=0`, `x=199`, `y=0`, and `y=159`;
- medium-gray interior lines every 20 pixels;
- ten columns by eight rows of nominally square 20-pixel cells.

The pattern directly exercises the first and last addressable pixel on every
LCD edge. All direct VRAM writes occur while LCD transfer is disabled with
`LCC[7]=0`; the completed page is enabled once and is never modified again.
Horizontal lines are written before vertical lines, so rendering requires no
VRAM readback. This is important because direct VRAM reads are not assumed to
return the previously written display data.

The build uses only the assembly source, shared register definitions, and the
C binaries from this repository. It does not use scripts, generated source,
graphical assets, HDK objects, or `data.bin`.

From the repository root:

```sh
make
make -C examples/edge_grid_test
make -C examples/edge_grid_test verify
```

Output:

```text
examples/edge_grid_test/build/edge_grid_test.tgc
```

The RTL verification launches the cartridge through the normal BIOS menu and
checks both the native VRAM layout and the displayed 200x160 frame. The expected
dot totals are 28,539 background, 2,745 gray, and 716 black perimeter pixels;
all 32,000 pixels must match exactly.
