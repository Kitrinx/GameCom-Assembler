; Shared helpers for Game.com hardware test ROM examples.
; These routines avoid BIOS drawing calls so tests can run from a plain cart
; entry point with only assembly and binary toolchain inputs.

InitTestSystem:
	di
	mov IE0,#00h
	mov IE1,#00h
	mov IR0,#00h
	mov IR1,#00h
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#00h
	mov SGDA,#80h
	mov TM0C,#00h
	mov TM1C,#00h
	mov MMU2,#20h
	mov MMU3,#21h
	mov MMU4,#22h
	mov LCC,#0b0h
	mov LCH,#07h
	mov LCV,#27h
	ret

ClearVramA:
	clr CommonFillByte
	movw rr2,#VRAM_A
	movw rr4,#VRAM_A_END
	call FillVramRange
	ret

ClearVramB:
	clr CommonFillByte
	movw rr2,#VRAM_B
	movw rr4,#VRAM_B_END
	call FillVramRange
	ret

ClearVramBoth:
	call ClearVramA
	call ClearVramB
	ret

FillVramA:
	mov CommonFillByte,r0
	movw rr2,#VRAM_A
	movw rr4,#VRAM_A_END
	call FillVramRange
	ret

FillVramB:
	mov CommonFillByte,r0
	movw rr2,#VRAM_B
	movw rr4,#VRAM_B_END
	call FillVramRange
	ret

FillVramRange:
	mov r0,CommonFillByte
FillVramRangeLoop:
	mov (rr2)+,r0
	cmpw rr2,rr4
	br ule,FillVramRangeLoop
	ret

DrawString:
	mov r0,(rr4)+
	cmp r0,#00h
	br eq,DrawStringDone
	call DrawChar
	br DrawString
DrawStringDone:
	ret

DrawChar:
	sub r0,#20h
	movw rr6,#FontTable
DrawCharSeek:
	cmp r0,#00h
	br eq,DrawCharGlyph
	addw rr6,#10
	dec r0
	br DrawCharSeek
DrawCharGlyph:
	mov CommonDrawCount,#5
DrawCharColumn:
	mov r1,(rr6)+
	mov @rr2,r1
	mov r1,(rr6)+
	mov 1(rr2),r1
	addw rr2,#40
	dec CommonDrawCount
	br nz,DrawCharColumn
	clr r1
	mov @rr2,r1
	mov 1(rr2),r1
	addw rr2,#40
	ret

DrawSpace:
	mov r0,#20h
	call DrawChar
	ret

DrawHexNibble:
	and r0,#0fh
	cmp r0,#10
	br ult,DrawHexDigit
	add r0,#37h
	call DrawChar
	ret
DrawHexDigit:
	add r0,#30h
	call DrawChar
	ret

DrawHexByte:
	mov CommonHexSave,r0
	srl r0
	srl r0
	srl r0
	srl r0
	call DrawHexNibble
	mov r0,CommonHexSave
	call DrawHexNibble
	ret

DrawHexWord:
	mov CommonHexSave,r0
	mov r0,r1
	call DrawHexByte
	mov r0,CommonHexSave
	call DrawHexByte
	ret

DelayInput:
	mov r6,#8
DelayInputLoop:
	dec r6
	br nz,DelayInputLoop
	ret

DelayTiny:
	mov r6,#40
DelayTinyLoop:
	dec r6
	br nz,DelayTinyLoop
	ret

DelayWindow:
	movw rr6,#4000h
DelayWindowLoop:
	decw rr6
	br nz,DelayWindowLoop
	ret

SetupButtonScan:
	mov P0,#0ffh
	mov P1,#0ffh
	mov P2,#0ffh
	mov P0C,#55h
	mov P1C,#55h
	mov P2C,#95h
	ret

ScanMainButtons:
	mov P2,#0ffh
	call DelayInput
	mov P2,#7fh
	call DelayInput
	ret

WaitButtonsReleased:
	call ScanMainButtons
	cmp P0,#0ffh
	br nz,WaitButtonsReleased
	cmp P1,#0ffh
	br nz,WaitButtonsReleased
	ret

WaitPagePress:
	call WaitButtonsReleased
WaitPagePressLoop:
	call ScanMainButtons
	cmp P0,#0ffh
	br nz,WaitPageSeen
	cmp P1,#0ffh
	br nz,WaitPageSeen
	br WaitPagePressLoop
WaitPageSeen:
	call WaitButtonsReleased
	ret

DmaHaltWait:
	push IE0
	push IE1
	di
	mov IE0,#80h
	mov IE1,#00h
	halt
	bclr IR0,#7
	pop IE1
	pop IE0
	ret

DrawFooter:
	movw rr2,#Line12
	movw rr4,#StrFooter
	call DrawString
	ret

StrFooter:
	dm 'BUTTON NEXT'
	db 0

FontTable:
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 20
	db 00h,00h,00h,00h,0ffh,0cch,00h,00h,00h,00h	; 21
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 22
	db 30h,0c0h,0ffh,0f0h,30h,0c0h,0ffh,0f0h,30h,0c0h	; 23
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 24
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 25
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 26
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 27
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 28
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 29
	db 33h,30h,0fh,0c0h,3fh,0f0h,0fh,0c0h,33h,30h	; 2A
	db 03h,00h,03h,00h,3fh,0f0h,03h,00h,03h,00h	; 2B
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 2C
	db 03h,00h,03h,00h,03h,00h,03h,00h,00h,00h	; 2D
	db 00h,00h,00h,3ch,00h,3ch,00h,00h,00h,00h	; 2E
	db 00h,0c0h,03h,00h,0ch,00h,30h,00h,0c0h,00h	; 2F
	db 3fh,0f0h,0c0h,0cch,0c3h,0ch,0cch,0ch,3fh,0f0h	; 30
	db 00h,00h,30h,0ch,0ffh,0fch,00h,0ch,00h,00h	; 31
	db 30h,0ch,0c0h,3ch,0c0h,0cch,0c3h,0ch,3ch,0ch	; 32
	db 0c0h,0ch,0c3h,0ch,0c3h,0ch,0c3h,0ch,3ch,0f0h	; 33
	db 03h,0c0h,0ch,0c0h,30h,0c0h,0ffh,0fch,00h,0c0h	; 34
	db 0ffh,0ch,0c3h,0ch,0c3h,0ch,0c3h,0ch,0c0h,0f0h	; 35
	db 0fh,0f0h,33h,0ch,0c3h,0ch,0c3h,0ch,00h,0f0h	; 36
	db 0c0h,00h,0c0h,0fch,0c3h,00h,0cch,00h,0f0h,00h	; 37
	db 3ch,0f0h,0c3h,0ch,0c3h,0ch,0c3h,0ch,3ch,0f0h	; 38
	db 3ch,0ch,0c3h,0ch,0c3h,0ch,0c3h,30h,3fh,0c0h	; 39
	db 00h,00h,00h,00h,3ch,0f0h,00h,00h,00h,00h	; 3A
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 3B
	db 03h,00h,0ch,0c0h,30h,30h,0c0h,0ch,00h,00h	; 3C
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 3D
	db 0c0h,0ch,30h,30h,0ch,0c0h,03h,00h,00h,00h	; 3E
	db 30h,00h,0c0h,00h,0c0h,0cch,0c3h,00h,3ch,00h	; 3F
	db 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	; 40
	db 3fh,0fch,0c3h,00h,0c3h,00h,0c3h,00h,3fh,0fch	; 41
	db 0ffh,0fch,0c3h,0ch,0c3h,0ch,0c3h,0ch,3ch,0f0h	; 42
	db 3fh,0f0h,0c0h,0ch,0c0h,0ch,0c0h,0ch,0c0h,0ch	; 43
	db 0ffh,0fch,0c0h,0ch,0c0h,0ch,0c0h,0ch,3fh,0f0h	; 44
	db 0ffh,0fch,0c3h,0ch,0c3h,0ch,0c3h,0ch,0c0h,0ch	; 45
	db 0ffh,0fch,0c3h,00h,0c3h,00h,0c3h,00h,0c0h,00h	; 46
	db 3fh,0f0h,0c0h,0ch,0c0h,0ch,0c3h,0ch,0c3h,0fch	; 47
	db 0ffh,0fch,03h,00h,03h,00h,03h,00h,0ffh,0fch	; 48
	db 00h,00h,0c0h,0ch,0ffh,0fch,0c0h,0ch,00h,00h	; 49
	db 00h,0f0h,00h,0ch,00h,0ch,00h,0ch,0ffh,0f0h	; 4A
	db 0ffh,0fch,03h,00h,0ch,0c0h,30h,30h,0c0h,0ch	; 4B
	db 0ffh,0fch,00h,0ch,00h,0ch,00h,0ch,00h,0ch	; 4C
	db 0ffh,0fch,30h,00h,0fh,00h,30h,00h,0ffh,0fch	; 4D
	db 0ffh,0fch,30h,00h,0ch,00h,03h,00h,0ffh,0fch	; 4E
	db 3fh,0f0h,0c0h,0ch,0c0h,0ch,0c0h,0ch,3fh,0f0h	; 4F
	db 0ffh,0fch,0c3h,00h,0c3h,00h,0c3h,00h,3ch,00h	; 50
	db 3fh,0f0h,0c0h,0ch,0c0h,0cch,0c0h,30h,3fh,0cch	; 51
	db 0ffh,0fch,0c3h,00h,0c3h,0c0h,0c3h,30h,3ch,0ch	; 52
	db 3ch,0ch,0c3h,0ch,0c3h,0ch,0c3h,0ch,0c0h,0f0h	; 53
	db 0c0h,00h,0c0h,00h,0ffh,0fch,0c0h,00h,0c0h,00h	; 54
	db 0ffh,0f0h,00h,0ch,00h,0ch,00h,0ch,0ffh,0f0h	; 55
	db 0ffh,0c0h,00h,30h,00h,0ch,00h,30h,0ffh,0c0h	; 56
	db 0ffh,0f0h,00h,0ch,03h,0f0h,00h,0ch,0ffh,0f0h	; 57
	db 0f0h,3ch,0ch,0c0h,03h,00h,0ch,0c0h,0f0h,3ch	; 58
	db 0f0h,00h,0ch,00h,03h,0fch,0ch,00h,0f0h,00h	; 59
	db 0c0h,3ch,0c0h,0cch,0c3h,0ch,0cch,0ch,0f0h,0ch	; 5A
