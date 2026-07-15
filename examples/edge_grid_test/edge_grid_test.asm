; Game.com native-LCD edge and grid test pattern.
; Build path: gamecom-as -> gamecom-romtool. No generated source, scripts,
; graphical assets, HDK objects, or data.bin.

	include "../common/gc_test_defs.inc"

	org 4000h

SPH		equ 1ch
SPL		equ 1dh
WDTC		equ 5fh

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'EDGEGRID '
	dw 1a18h
	db 99h
	db 00h
	dw 0000h

Entry:
	clr r0
	cmp r2,#00h
	br eq,EntryReturn
	cmp r2,#01h
	jmp eq,Main
	cmp r2,#02h
	br eq,EntryReturn
	cmp r2,#03h
	br eq,EntryReturn
	mov r0,#0ffh
EntryReturn:
	ret

Main:
	di
	; Do not inherit watchdog, register-bank, or stack state from the BIOS.
	mov WDTC,#00h
	mov PS0,#00h
	bset SYS,#6
	mov SPH,#03h
	mov SPL,#0f0h
	mov IE0,#00h
	mov IE1,#00h
	mov IR0,#00h
	mov IR1,#00h
	mov DMC,#00h
	mov TM0C,#00h
	mov TM1C,#00h
	mov SGC,#00h
	mov MMU2,#20h
	mov MMU3,#21h
	mov MMU4,#22h

	; LCC[7]=0 disables LCD transfer. Keep it disabled for every direct VRAM
	; write, then enable page A only after the complete frame is ready.
	mov LCC,#30h
	mov LCH,#07h
	mov LCV,#27h
	call ClearFrame
	call DrawHorizontalGrid
	call DrawVerticalGrid
	mov LCC,#0b0h

PatternLoop:
	br PatternLoop

ClearFrame:
	movw rr2,#VRAM_A
	movw rr4,#VRAM_A_END
	clr r0
ClearFrameLoop:
	mov (rr2)+,r0
	cmpw rr2,rr4
	br ule,ClearFrameLoop
	ret

DrawVerticalGrid:
	; Physical left edge, x=0.
	movw rr2,#0a000h
	mov r0,#0ffh
	call FillColumn

	; Interior columns x=20,40,...,180. AA draws shade 2 in all four
	; packed vertical pixels. EA/AB keep the top and bottom endpoint pixels
	; black; the next column is 800 bytes away.
	movw rr2,#0a320h
	mov r7,#9
DrawInteriorColumnLoop:
	call FillInteriorColumn
	addw rr2,#02f8h
	dec r7
	br nz,DrawInteriorColumnLoop

	; Physical right edge, x=199.
	movw rr2,#0bf18h
	mov r0,#0ffh
	call FillColumn
	ret

FillInteriorColumn:
	mov r0,#0eah
	mov (rr2)+,r0
	mov r0,#0aah
	mov r6,#38
FillInteriorColumnLoop:
	mov (rr2)+,r0
	dec r6
	br nz,FillInteriorColumnLoop
	mov r0,#0abh
	mov (rr2)+,r0
	ret

FillColumn:
	mov r6,#40
FillColumnLoop:
	mov (rr2)+,r0
	dec r6
	br nz,FillColumnLoop
	ret

DrawHorizontalGrid:
	; Top edge y=0 uses bits 7:6 of byte 0 in every X column.
	movw rr2,#0a000h
	mov r0,#0c0h
	call WriteHorizontalLine

	; Interior lines y=20,40,...,140 all occupy bits 7:6 because they
	; are multiples of four. 80h draws shade 2 without altering neighbors.
	movw rr2,#0a005h
	mov r0,#080h
	call WriteHorizontalLine
	movw rr2,#0a00ah
	call WriteHorizontalLine
	movw rr2,#0a00fh
	call WriteHorizontalLine
	movw rr2,#0a014h
	call WriteHorizontalLine
	movw rr2,#0a019h
	call WriteHorizontalLine
	movw rr2,#0a01eh
	call WriteHorizontalLine
	movw rr2,#0a023h
	call WriteHorizontalLine

	; Bottom edge y=159 uses bits 1:0 of byte 39 in every X column.
	movw rr2,#0a027h
	mov r0,#03h
	call WriteHorizontalLine
	ret

WriteHorizontalLine:
	mov r7,#200
WriteHorizontalLineLoop:
	mov @rr2,r0
	addw rr2,#40
	dec r7
	br nz,WriteHorizontalLineLoop
	ret

	end
