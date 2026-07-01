; Game.com sound characterization/recording test, converted to SM8521 assembly.
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
	dm 'SND REC  '
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

RecordingRunOnce:
	call ClearVram
	movw rr4,#SpansRecording
	call RenderSpans
	call ClearState
	call RecordingSuite
	call MuteSound
	movw rr4,#SpansProgressDone
	call RenderSpans
	call WaitRestartButton
	jmp RecordingRunOnce

	include "sound_routines.inc"
	include "sound_recording_routines.inc"
	include "sound_data_common.inc"
	include "sound_recording_data.inc"

	org 5800h
SpansRecording:
SpansProgress01	equ SpansRecording + 1088h
SpansProgress02	equ SpansRecording + 12b1h
SpansProgress03	equ SpansRecording + 14dah
SpansProgress04	equ SpansRecording + 1703h
SpansProgress05	equ SpansRecording + 192ch
SpansProgress06	equ SpansRecording + 1b55h
SpansProgress07	equ SpansRecording + 1d7eh
SpansProgress08	equ SpansRecording + 1fa7h
SpansProgress09	equ SpansRecording + 21d0h
SpansProgress10	equ SpansRecording + 23f9h
SpansProgress11	equ SpansRecording + 2622h
SpansProgress12	equ SpansRecording + 284bh
SpansProgressDone	equ SpansRecording + 2a74h
	incbin "assets/recording_spans.bin"

	end
