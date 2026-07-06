# DMA/LCDC Visual Test

Builds a BIOS-launchable Game.com hardware test ROM for visual DMA and LCDC
characterization.

Press any main button to advance pages. The ROM uses visible LCD output as the
primary result, so it is useful on hardware even when CPU-visible VRAM reads are
not trustworthy.

Pages:

- `0`: index.
- `1`: VRAM-to-VRAM DMA with identity and reversed `DMPL` palette mapping.
- `2`: compound transparent-zero copy beside overwrite copy.
- `3`: `DMVP` hidden-page destination test; the visible page is switched to
  page 1 after DMA.
- `4`: same-page overlap plus reverse-X source stepping.
- `5..8`: LCDC gradation pages using `LCC=80/90/A0/B0`.
- `9`: geometry/divider page using `LCH=27`, `LCV=37`, and `LCC=B8`.

The build uses only `gamecom-as` and `gamecom-romtool`.
