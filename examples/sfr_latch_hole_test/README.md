# SFR Latch And Hole Test

Builds a BIOS-launchable Game.com hardware test ROM that writes selected SFRs
and SFR holes, then displays raw readback bytes.

Rows are shown as:

```text
AA NAME R0 R1 R2 R3
```

`AA` is the direct/SFR address. `R0..R3` are the raw readbacks after the four
patterns stored for that row in `SfrTestTable`. Most rows use
`00 FF AA 55`; side-effect-sensitive rows use safer patterns noted in the ASM.

The ROM intentionally reports raw observations. It does not treat the current
RTL, MAME, or the datasheet interpretation as final truth.

Press any main button to page through the table.
