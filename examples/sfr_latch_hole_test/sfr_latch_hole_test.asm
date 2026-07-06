; Game.com SFR raw latch and hole hardware test.
; Build path: gamecom-as -> gamecom-romtool. No scripts or HDK data.bin.

	include "../common/gc_test_defs.inc"

	org 4000h

CurrentPage	equ 0d8h
RowIndex	equ 0d9h
PageBase	equ 0dah
ResultPtr	equ 0dch
TmpAddr		equ 0deh
TmpPattern	equ 0dfh
SfrCount	equ 0e0h

SfrResults	equ 0200h

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'SFR TEST '
	dw 1a18h
	db 97h
	db 00h
	dw 0000h

Entry:
	clr r0
	cmp r2,#00h
	br eq,EntryReturn
	cmp r2,#01h
	jmp eq,Main
	cmp r2,#02h
	br eq,EntryClose
	cmp r2,#03h
	br eq,EntryReturn
	mov r0,#0ffh
EntryReturn:
	ret
EntryClose:
	call QuiesceSideEffects
	clr r0
	ret

Main:
	call InitTestSystem
	call ClearVramBoth
	movw rr2,#Line1
	movw rr4,#StrRunning
	call DrawString
	call RunSfrTests
	clr CurrentPage
	call RenderCurrentPage
	call SetupButtonScan
PageLoop:
	call WaitPagePress
	inc CurrentPage
	cmp CurrentPage,#09h
	br ult,PageNoWrap
	clr CurrentPage
PageNoWrap:
	call RenderCurrentPage
	br PageLoop

RunSfrTests:
	clr SfrCount
	movw rr4,#SfrTestTable
	movw ResultPtr,#SfrResults
RunSfrLoop:
	mov r0,(rr4)+
	cmp r0,#0ffh
	br eq,RunSfrDone
	mov TmpAddr,r0
	movw rr2,ResultPtr
	mov (rr2)+,r0
	mov r7,#04h
RunSfrPatternLoop:
	mov r0,(rr4)+
	mov TmpPattern,r0
	mov r2,TmpAddr
	mov @r2,r0
	call DelayTiny
	mov r2,TmpAddr
	mov r0,@r2
	mov (rr2)+,r0
	dec r7
	br nz,RunSfrPatternLoop
	movw ResultPtr,rr2
	inc SfrCount
	call QuiesceSideEffects
	br RunSfrLoop
RunSfrDone:
	call InitTestSystem
	ret

QuiesceSideEffects:
	di
	mov TM0C,#00h
	mov TM1C,#00h
	mov IE0,#00h
	mov IE1,#00h
	mov IR0,#00h
	mov IR1,#00h
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#00h
	mov SGDA,#80h
	mov DMC,#00h
	mov LCC,#0b0h
	mov LCH,#07h
	mov LCV,#27h
	ret

RenderCurrentPage:
	call ClearVramA
	call ComputePageBase
	movw rr2,#Line0
	movw rr4,#StrTitle
	call DrawString
	mov r0,CurrentPage
	add r0,#30h
	call DrawChar
	movw rr2,#Line1
	movw rr4,#StrHead
	call DrawString
	mov r0,PageBase
	mov RowIndex,r0
	movw rr2,#Line2
	call DrawSfrRow
	inc RowIndex
	movw rr2,#Line3
	call DrawSfrRow
	inc RowIndex
	movw rr2,#Line4
	call DrawSfrRow
	inc RowIndex
	movw rr2,#Line5
	call DrawSfrRow
	inc RowIndex
	movw rr2,#Line6
	call DrawSfrRow
	inc RowIndex
	movw rr2,#Line7
	call DrawSfrRow
	call DrawFooter
	ret

ComputePageBase:
	clr PageBase
	mov r1,CurrentPage
ComputePageLoop:
	cmp r1,#00h
	br eq,ComputePageDone
	add PageBase,#06h
	dec r1
	br ComputePageLoop
ComputePageDone:
	ret

DrawSfrRow:
	mov r0,RowIndex
	cmp r0,SfrCount
	br uge,DrawSfrBlank
	movw rr6,#SfrResults
	mov r1,RowIndex
DrawSfrSeek:
	cmp r1,#00h
	br eq,DrawSfrFound
	addw rr6,#5
	dec r1
	br DrawSfrSeek
DrawSfrFound:
	mov r0,@rr6
	call DrawHexByte
	call DrawSpace
	call DrawSfrName
	call DrawSpace
	mov r0,1(rr6)
	call DrawHexByte
	call DrawSpace
	mov r0,2(rr6)
	call DrawHexByte
	call DrawSpace
	mov r0,3(rr6)
	call DrawHexByte
	call DrawSpace
	mov r0,4(rr6)
	call DrawHexByte
	ret
DrawSfrBlank:
	ret

DrawSfrName:
	push r6
	push r7
	movw rr4,#SfrNamePtrs
	mov r1,RowIndex
DrawSfrNameSeek:
	cmp r1,#00h
	br eq,DrawSfrNameFound
	addw rr4,#2
	dec r1
	br DrawSfrNameSeek
DrawSfrNameFound:
	movw rr4,(rr4)+
	call DrawString
	pop r7
	pop r6
	ret

StrRunning:	dm 'RUNNING SFR TEST'
	db 0
StrTitle:	dm 'SFR RAW PAGE '
	db 0
StrHead:	dm 'AD NAME R0 R1 R2 R3'
	db 0

; Table format: address, pattern0, pattern1, pattern2, pattern3.
; Most rows use 00/FF/AA/55. DMC avoids bit 7 so it never arms DMA.
; LCC keeps bit 7 set in every pattern used here so the display can recover.
SfrTestTable:
	db 10h,00h,0ffh,0aah,55h
	db 11h,00h,0ffh,0aah,55h
	db 12h,00h,0ffh,0aah,55h
	db 13h,00h,0ffh,0aah,55h
	db 30h,80h,0b0h,0f0h,0aah
	db 31h,00h,0ffh,0aah,55h
	db 32h,00h,0ffh,0aah,55h
	db 34h,00h,7fh,2ah,55h
	db 35h,00h,0ffh,0aah,55h
	db 36h,00h,0ffh,0aah,55h
	db 37h,00h,0ffh,0aah,55h
	db 38h,00h,0ffh,0aah,55h
	db 39h,00h,0ffh,0aah,55h
	db 3ah,00h,0ffh,0aah,55h
	db 3bh,00h,0ffh,0aah,55h
	db 3ch,00h,0ffh,0aah,55h
	db 3dh,00h,0ffh,0aah,55h
	db 40h,00h,7fh,2ah,55h
	db 42h,00h,0ffh,0aah,55h
	db 44h,00h,0ffh,0aah,55h
	db 46h,00h,0ffh,0aah,55h
	db 47h,00h,0ffh,0aah,55h
	db 48h,00h,0ffh,0aah,55h
	db 49h,00h,0ffh,0aah,55h
	db 4ah,00h,0ffh,0aah,55h
	db 4ch,00h,0ffh,0aah,55h
	db 4dh,00h,0ffh,0aah,55h
	db 4eh,00h,0ffh,0aah,55h
	db 50h,00h,7fh,88h,0cch
	db 51h,00h,0ffh,0aah,55h
	db 52h,00h,7fh,88h,0cch
	db 53h,00h,0ffh,0aah,55h
	db 54h,00h,0ffh,0aah,55h
	db 18h,00h,0ffh,0aah,55h
	db 1bh,00h,0ffh,0aah,55h
	db 2ah,00h,0ffh,0aah,55h
	db 2fh,00h,0ffh,0aah,55h
	db 33h,00h,0ffh,0aah,55h
	db 3eh,00h,0ffh,0aah,55h
	db 3fh,00h,0ffh,0aah,55h
	db 41h,00h,0ffh,0aah,55h
	db 43h,00h,0ffh,0aah,55h
	db 45h,00h,0ffh,0aah,55h
	db 4bh,00h,0ffh,0aah,55h
	db 4fh,00h,0ffh,0aah,55h
	db 55h,00h,0ffh,0aah,55h
	db 56h,00h,0ffh,0aah,55h
	db 57h,00h,0ffh,0aah,55h
	db 58h,00h,0ffh,0aah,55h
	db 59h,00h,0ffh,0aah,55h
	db 5ah,00h,0ffh,0aah,55h
	db 5bh,00h,0ffh,0aah,55h
	db 5ch,00h,0ffh,0aah,55h
	db 5dh,00h,0ffh,0aah,55h
	db 0ffh

SfrNamePtrs:
	dw NameIE0,NameIE1,NameIR0,NameIR1,NameLCC,NameLCH
	dw NameLCV,NameDMC,NameDMX1,NameDMY1,NameDMDX,NameDMDY
	dw NameDMX2,NameDMY2,NameDMPL,NameDMBR,NameDMVP,NameSGC
	dw NameSG0L,NameSG1L,NameS0TH,NameS0TL,NameS1TH,NameS1TL
	dw NameSG2L,NameS2TH,NameS2TL,NameSGDA,NameTM0C,NameTM0D
	dw NameTM1C,NameTM1D,NameCLKT,NameH18,NameH1B,NameH2A
	dw NameH2F,NameH33,NameH3E,NameH3F,NameH41,NameH43
	dw NameH45,NameH4B,NameH4F,NameH55,NameH56,NameH57
	dw NameH58,NameH59,NameH5A,NameH5B,NameH5C,NameH5D

NameIE0:	dm 'IE0'
	db 0
NameIE1:	dm 'IE1'
	db 0
NameIR0:	dm 'IR0'
	db 0
NameIR1:	dm 'IR1'
	db 0
NameLCC:	dm 'LCC'
	db 0
NameLCH:	dm 'LCH'
	db 0
NameLCV:	dm 'LCV'
	db 0
NameDMC:	dm 'DMC'
	db 0
NameDMX1:	dm 'DMX1'
	db 0
NameDMY1:	dm 'DMY1'
	db 0
NameDMDX:	dm 'DMDX'
	db 0
NameDMDY:	dm 'DMDY'
	db 0
NameDMX2:	dm 'DMX2'
	db 0
NameDMY2:	dm 'DMY2'
	db 0
NameDMPL:	dm 'DMPL'
	db 0
NameDMBR:	dm 'DMBR'
	db 0
NameDMVP:	dm 'DMVP'
	db 0
NameSGC:	dm 'SGC'
	db 0
NameSG0L:	dm 'SG0L'
	db 0
NameSG1L:	dm 'SG1L'
	db 0
NameS0TH:	dm 'S0TH'
	db 0
NameS0TL:	dm 'S0TL'
	db 0
NameS1TH:	dm 'S1TH'
	db 0
NameS1TL:	dm 'S1TL'
	db 0
NameSG2L:	dm 'SG2L'
	db 0
NameS2TH:	dm 'S2TH'
	db 0
NameS2TL:	dm 'S2TL'
	db 0
NameSGDA:	dm 'SGDA'
	db 0
NameTM0C:	dm 'TM0C'
	db 0
NameTM0D:	dm 'TM0D'
	db 0
NameTM1C:	dm 'TM1C'
	db 0
NameTM1D:	dm 'TM1D'
	db 0
NameCLKT:	dm 'CLKT'
	db 0
NameH18:	dm 'H18'
	db 0
NameH1B:	dm 'H1B'
	db 0
NameH2A:	dm 'H2A'
	db 0
NameH2F:	dm 'H2F'
	db 0
NameH33:	dm 'H33'
	db 0
NameH3E:	dm 'H3E'
	db 0
NameH3F:	dm 'H3F'
	db 0
NameH41:	dm 'H41'
	db 0
NameH43:	dm 'H43'
	db 0
NameH45:	dm 'H45'
	db 0
NameH4B:	dm 'H4B'
	db 0
NameH4F:	dm 'H4F'
	db 0
NameH55:	dm 'H55'
	db 0
NameH56:	dm 'H56'
	db 0
NameH57:	dm 'H57'
	db 0
NameH58:	dm 'H58'
	db 0
NameH59:	dm 'H59'
	db 0
NameH5A:	dm 'H5A'
	db 0
NameH5B:	dm 'H5B'
	db 0
NameH5C:	dm 'H5C'
	db 0
NameH5D:	dm 'H5D'
	db 0

	include "../common/gc_test_common.inc"

	end
