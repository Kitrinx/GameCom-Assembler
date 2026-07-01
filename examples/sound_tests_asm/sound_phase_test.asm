; Game.com sound phase test, converted to proper SM8521 assembly.
; Build path: gamecom-as -> gamecom-romtool. No generator script or data.bin.

	include "sound_defs.inc"

	org 4000h

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'SND PHASE'
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
	call MuteSound
	clr r0
	ret

Main:
	di
	mov MMU2,#20h
	mov MMU3,#21h
	mov MMU4,#22h

RunOnce:
PhaseStart0:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase0
	call RenderSpans
	call ClearState
	call HoldShort

PhaseStart1:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase1
	call RenderSpans
	call LoadSgx
	call HoldShort

PhaseStart2:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase2
	call RenderSpans
	call InitSgx
	call HoldShort

PhaseStart3:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase3
	call RenderSpans
	call PlaySgx

PhaseStart4:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase4
	call RenderSpans
	call PlayNoise

PhaseStart5:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase5
	call RenderSpans
	call LoadSgda
	call HoldShort

PhaseStart6:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase6
	call RenderSpans
	call InitSgda
	call HoldShort

PhaseStart7:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase7
	call RenderSpans
	call PlaySgda

PhaseStart8:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase8
	call RenderSpans
	call PlayDacOverflowHi

PhaseStart9:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase9
	call RenderSpans
	call PlayDacOverflowLo

PhaseStart10:
	call ClearVram
	movw rr4,#SpansCommon
	call RenderSpans
	movw rr4,#SpansPhase10
	call RenderSpans
	call MuteSound
	call HoldShort
	call WaitRestartButton
	jmp RunOnce

	include "sound_routines.inc"
	include "sound_data_common.inc"

	org 43dfh
SpansCommon:
SpansPhase0	equ SpansCommon + 0935h
SpansPhase1	equ SpansCommon + 0f70h
SpansPhase2	equ SpansCommon + 16c3h
SpansPhase3	equ SpansCommon + 1e03h
SpansPhase4	equ SpansCommon + 2564h
SpansPhase5	equ SpansCommon + 2bf9h
SpansPhase6	equ SpansCommon + 346eh
SpansPhase7	equ SpansCommon + 3bddh
SpansPhase8	equ SpansCommon + 4409h
SpansPhase9	equ SpansCommon + 4adah
SpansPhase10	equ SpansCommon + 51e9h
	incbin "assets/phase_spans.bin"

	end
