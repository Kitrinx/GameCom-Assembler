; Game.com timer prescale and IRQ-boundary hardware test.
; Build path: gamecom-as -> gamecom-romtool. No scripts or HDK data.bin.

	include "../common/gc_test_defs.inc"

	org 4000h

CurrentPage	equ 0d8h
RowIndex	equ 0d9h
ResultPtr	equ 0dah
LoopRemain	equ 0dch
Elapsed		equ 0deh
TmpByte		equ 0e0h
SavedA6		equ 0e1h
SavedVec	equ 0e2h
IsrCount	equ 0e4h
IsrLastIr1	equ 0e5h

PrescaleResults	equ 0200h
PrescaleIr	equ 0210h
WriteResults	equ 0220h
IrqResults	equ 0230h

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'TIMERIRQ '
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
	di
	mov TM0C,#00h
	mov TM1C,#00h
	mov IE0,#00h
	mov IE1,#00h
	clr r0
	ret

Main:
	call InitTestSystem
	call ClearVramBoth
	movw rr2,#Line1
	movw rr4,#StrRunning
	call DrawString
	call RunTimerTests
	clr CurrentPage
	call RenderCurrentPage
	call SetupButtonScan
PageLoop:
	call WaitPagePress
	inc CurrentPage
	cmp CurrentPage,#03h
	br ult,PageNoWrap
	clr CurrentPage
PageNoWrap:
	call RenderCurrentPage
	br PageLoop

RunTimerTests:
	call RunPrescaleSweep
	call RunWriteBehavior
	call RunTimer1IrqProbe
	ret

RunPrescaleSweep:
	clr RowIndex
	movw ResultPtr,#PrescaleResults
RunPrescaleLoop:
	call MeasureTimer0Selector
	movw rr2,ResultPtr
	mov r0,Elapsed
	mov (rr2)+,r0
	mov r0,Elapsed+1
	mov (rr2)+,r0
	movw ResultPtr,rr2
	movw rr2,#PrescaleIr
	mov r1,RowIndex
RunIrSeek:
	cmp r1,#00h
	br eq,RunIrFound
	incw rr2
	dec r1
	br RunIrSeek
RunIrFound:
	mov r0,TmpByte
	mov @rr2,r0
	inc RowIndex
	cmp RowIndex,#08h
	br nz,RunPrescaleLoop
	ret

MeasureTimer0Selector:
	mov TM0C,#00h
	mov IR0,#00h
	mov TM0D,#0feh
	mov r0,RowIndex
	or r0,#80h
	mov TM0C,r0
	movw rr6,#0ffffh
MeasureTimer0Loop:
	mov r0,IR0
	and r0,#40h
	br nz,MeasureTimer0Done
	decw rr6
	br nz,MeasureTimer0Loop
MeasureTimer0Done:
	mov TM0C,#00h
	mov r0,IR0
	mov TmpByte,r0
	mov IR0,#00h
	movw LoopRemain,rr6
	movw Elapsed,#0ffffh
	subw Elapsed,LoopRemain
	ret

RunWriteBehavior:
	mov TM0C,#00h
	mov IR0,#00h
	mov TM0D,#0f0h
	mov TM0C,#80h
	call DelayTiny
	mov r0,TM0D
	mov WriteResults+0,r0
	mov TM0D,#33h
	mov r0,TM0D
	mov WriteResults+1,r0
	call DelayTiny
	mov r0,TM0D
	mov WriteResults+2,r0
	mov TM0C,#0cch
	mov r0,TM0C
	mov WriteResults+3,r0
	mov TM0C,#00h
	mov IR0,#00h
	ret

RunTimer1IrqProbe:
	call InstallTimer1Vector
	call ProbeTimer1Enabled
	call ProbeTimer1MaskedThenEnable
	call RestoreTimer1Vector
	ret

InstallTimer1Vector:
	mov r0,BIOS_TIMER_BANK
	mov SavedA6,r0
	movw rr2,#0102h
	movw rr0,@rr2
	movw SavedVec,rr0
	mov BIOS_TIMER_BANK,#20h
	movw rr0,#Timer1UserIsr
	movw @rr2,rr0
	ret

RestoreTimer1Vector:
	di
	mov TM1C,#00h
	mov IE1,#00h
	mov IR1,#00h
	mov r0,SavedA6
	mov BIOS_TIMER_BANK,r0
	movw rr2,#0102h
	movw rr0,SavedVec
	movw @rr2,rr0
	ret

ProbeTimer1Enabled:
	clr IsrCount
	clr IsrLastIr1
	mov IR1,#00h
	mov TM1C,#00h
	mov TM1D,#0feh
	mov IE1,#40h
	mov PS0,#00h
	ei
	mov TM1C,#80h
	call DelayWindow
	di
	mov TM1C,#00h
	mov IE1,#00h
	mov r0,IsrCount
	mov IrqResults+0,r0
	mov r0,IsrLastIr1
	mov IrqResults+1,r0
	mov r0,IR1
	mov IrqResults+2,r0
	mov IR1,#00h
	ret

ProbeTimer1MaskedThenEnable:
	clr IsrCount
	clr IsrLastIr1
	mov IR1,#00h
	mov TM1C,#00h
	mov TM1D,#0feh
	mov IE1,#00h
	ei
	mov TM1C,#80h
	call DelayWindow
	di
	mov TM1C,#00h
	mov r0,IsrCount
	mov IrqResults+3,r0
	mov r0,IR1
	mov IrqResults+4,r0
	mov IE1,#40h
	mov PS0,#00h
	ei
	call DelayWindow
	di
	mov IE1,#00h
	mov r0,IsrCount
	mov IrqResults+5,r0
	mov r0,IR1
	mov IrqResults+6,r0
	mov IR1,#00h
	ret

Timer1UserIsr:
	inc IsrCount
	mov r0,IR1
	mov IsrLastIr1,r0
	mov IR1,#00h
	mov PS0,#38h
	jmp 2142h

RenderCurrentPage:
	call ClearVramA
	mov r0,CurrentPage
	cmp r0,#00h
	jmp eq,RenderPrescalePage
	cmp r0,#01h
	jmp eq,RenderIrqPage
	jmp RenderNotesPage

RenderPrescalePage:
	movw rr2,#Line0
	movw rr4,#StrTitle0
	call DrawString
	movw rr2,#Line1
	movw rr4,#StrHead0
	call DrawString
	mov RowIndex,#00h
	movw rr2,#Line2
	call DrawPrescaleRow
	mov RowIndex,#01h
	movw rr2,#Line3
	call DrawPrescaleRow
	mov RowIndex,#02h
	movw rr2,#Line4
	call DrawPrescaleRow
	mov RowIndex,#03h
	movw rr2,#Line5
	call DrawPrescaleRow
	mov RowIndex,#04h
	movw rr2,#Line6
	call DrawPrescaleRow
	mov RowIndex,#05h
	movw rr2,#Line7
	call DrawPrescaleRow
	mov RowIndex,#06h
	movw rr2,#Line8
	call DrawPrescaleRow
	mov RowIndex,#07h
	movw rr2,#Line9
	call DrawPrescaleRow
	call DrawFooter
	ret

DrawPrescaleRow:
	mov r0,RowIndex
	add r0,#30h
	call DrawChar
	call DrawSpace
	movw rr6,#PrescaleResults
	mov r1,RowIndex
DrawPrescaleSeek:
	cmp r1,#00h
	br eq,DrawPrescaleFound
	addw rr6,#2
	dec r1
	br DrawPrescaleSeek
DrawPrescaleFound:
	mov r0,@rr6
	mov r1,1(rr6)
	call DrawHexWord
	call DrawSpace
	movw rr6,#PrescaleIr
	mov r1,RowIndex
DrawPrescaleIrSeek:
	cmp r1,#00h
	br eq,DrawPrescaleIrFound
	incw rr6
	dec r1
	br DrawPrescaleIrSeek
DrawPrescaleIrFound:
	mov r0,@rr6
	call DrawHexByte
	ret

RenderIrqPage:
	movw rr2,#Line0
	movw rr4,#StrTitle1
	call DrawString
	movw rr2,#Line2
	movw rr4,#StrWrite
	call DrawString
	movw rr2,#Line3
	mov r0,WriteResults+0
	call DrawHexByte
	call DrawSpace
	mov r0,WriteResults+1
	call DrawHexByte
	call DrawSpace
	mov r0,WriteResults+2
	call DrawHexByte
	call DrawSpace
	mov r0,WriteResults+3
	call DrawHexByte
	movw rr2,#Line5
	movw rr4,#StrIrqEn
	call DrawString
	mov r0,IrqResults+0
	call DrawHexByte
	call DrawSpace
	mov r0,IrqResults+1
	call DrawHexByte
	call DrawSpace
	mov r0,IrqResults+2
	call DrawHexByte
	movw rr2,#Line6
	movw rr4,#StrIrqMask
	call DrawString
	mov r0,IrqResults+3
	call DrawHexByte
	call DrawSpace
	mov r0,IrqResults+4
	call DrawHexByte
	movw rr2,#Line7
	movw rr4,#StrIrqPend
	call DrawString
	mov r0,IrqResults+5
	call DrawHexByte
	call DrawSpace
	mov r0,IrqResults+6
	call DrawHexByte
	movw rr2,#Line9
	movw rr4,#StrVector
	call DrawString
	call DrawFooter
	ret

RenderNotesPage:
	movw rr2,#Line0
	movw rr4,#StrTitle2
	call DrawString
	movw rr2,#Line2
	movw rr4,#StrNote0
	call DrawString
	movw rr2,#Line3
	movw rr4,#StrNote1
	call DrawString
	movw rr2,#Line5
	movw rr4,#StrNote2
	call DrawString
	movw rr2,#Line6
	movw rr4,#StrNote3
	call DrawString
	movw rr2,#Line8
	movw rr4,#StrNote4
	call DrawString
	movw rr2,#Line9
	movw rr4,#StrNote5
	call DrawString
	call DrawFooter
	ret

StrRunning:	dm 'RUNNING TIMER TEST'
	db 0
StrTitle0:	dm 'TM0 PRESCALE'
	db 0
StrHead0:	dm 'SEL ELAP IR'
	db 0
StrTitle1:	dm 'TM0 WRITE T1 IRQ'
	db 0
StrWrite:	dm 'TM0D BEFORE WRITE AFTER TM0C'
	db 0
StrIrqEn:	dm 'EN CNT LASTIR NOW '
	db 0
StrIrqMask:	dm 'MASK CNT IR '
	db 0
StrIrqPend:	dm 'PEND EN CNT IR '
	db 0
StrVector:	dm 'T1 VECTOR 0102 BANK A6'
	db 0
StrTitle2:	dm 'TIMER NOTES'
	db 0
StrNote0:	dm 'TM0D START FE WAIT IR0'
	db 0
StrNote1:	dm 'ELAP IS LOOP COUNT'
	db 0
StrNote2:	dm 'SEL0 FAST SEL1 1024'
	db 0
StrNote3:	dm 'THEN 2048 ETC CAND'
	db 0
StrNote4:	dm 'MASK ROW SHOWS PENDING'
	db 0
StrNote5:	dm 'PEND EN SHOWS RETAIN'
	db 0

	include "../common/gc_test_common.inc"

	end
