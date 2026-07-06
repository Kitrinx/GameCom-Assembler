# Timer IRQ Boundary Test

Builds a BIOS-launchable Game.com hardware test ROM for timer prescale and
Timer 1 interrupt-boundary behavior.

The ROM reports raw observations instead of declaring current emulator/RTL
behavior as truth. Press any main button to advance pages.

Pages:

- `TM0 PRESCALE`: Timer 0 selector sweep. `ELAP` is the loop count until
  `IR0[6]` became visible after arming the timer with `TM0D=FE`.
- `TM0 WRITE T1 IRQ`: Timer 0 running-write samples plus Timer 1 ISR counts.
  The Timer 1 path installs a cart ISR through the external-BIOS RAM dispatch
  vector at `0102h` and tail-jumps back to the BIOS restore/`iret` path.
- `NOTES`: reminder of the candidate selector table and what the pending-enable
  row is intended to expose.

The build uses only `gamecom-as` and `gamecom-romtool`.
