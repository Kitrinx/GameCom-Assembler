; Shared code for the ASM sound-test ROMs.

ClearRam:
	movw rr2,#0080h
	clr r0
ClearRamLoop:
	mov (rr2)+,r0
	cmpw rr2,#037fh
	br ule,ClearRamLoop
	ret

ClearVram:
	call BeginDirectVramWrite
	movw rr2,#0a000h
	clr r0
ClearVramLoop:
	mov (rr2)+,r0
	cmpw rr2,#0bf3fh
	br ule,ClearVramLoop
	call EndDirectVramWrite
	ret

RenderSpans:
	call BeginDirectVramWrite
RenderRecordLoop:
	mov r1,(rr4)+
	cmp r1,#0
	br eq,RenderDone
	mov r2,(rr4)+
	mov r3,(rr4)+
RenderCopyLoop:
	mov r0,(rr4)+
	mov (rr2)+,r0
	dec r1
	br nz,RenderCopyLoop
	br RenderRecordLoop
RenderDone:
	call EndDirectVramWrite
	ret

BeginDirectVramWrite:
	mov SoundSavedLcc,LCC
	bclr LCC,#7
	ret

EndDirectVramWrite:
	mov LCC,SoundSavedLcc
	ret

Copy16ToSfr:
Copy16Loop:
	mov r0,(rr4)+
	mov (r2)+,r0
	dec r1
	br nz,Copy16Loop
	ret

CopySamplesToRam:
	movw rr2,#0280h
	movw rr4,#DacSamples
	mov r1,#64
CopySamplesLoop:
	mov r0,(rr4)+
	mov (rr2)+,r0
	dec r1
	br nz,CopySamplesLoop
	ret

HoldShort:
	mov r6,#PHASE_HOLD_UNITS
	call HoldCommon
	ret

HoldPlay:
	mov r6,#PHASE_HOLD_UNITS
	call HoldCommon
	ret

HoldCommon:
HoldOuter:
	movw rr0,#PHASE_HOLD_INNER
HoldInner:
	decw rr0
	br nz,HoldInner
	dec r6
	br nz,HoldOuter
	ret

DelayDac:
	mov r6,#16
DelayDacLoop:
	dec r6
	br nz,DelayDacLoop
	ret

ClearState:
	di
	mov IE0,#00h
	mov IE1,#00h
	mov IR0,#00h
	mov IR1,#00h
	call ClearRam
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG0TH,#00h
	mov SG0TL,#00h
	mov SG1TH,#00h
	mov SG1TL,#00h
	mov SG2L,#00h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	mov TM0C,#00h
	mov TM0D,#00h
	mov TM1C,#00h
	mov TM1D,#00h
	mov CLKT,#00h
	mov LCC,#0b0h
	mov LCH,#07h
	mov LCV,#27h
	ret

LoadSgx:
	mov SGC,#00h
	movw rr4,#Sg0Wave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#Sg1Wave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#1fh
	mov SG1L,#18h
	mov SG2L,#00h
	mov SG0TH,#00h
	mov SG0TL,#0a0h
	mov SG1TH,#00h
	mov SG1TL,#0d5h
	ret

InitSgx:
	di
	mov SGC,#00h
	mov SGDA,#80h
	mov SG0TL,#0a0h
	mov SG1TL,#0d5h
	mov SGC,#83h
	ret

PlaySgx:
	call HoldPlay
	ret

PlayNoise:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#0ffh
	mov SGDA,#80h
	mov SGC,#84h
	call HoldPlay
	ret

LoadSgda:
	call CopySamplesToRam
	mov TM1D,#0d0h
	mov TM1C,#80h
	mov SG0TH,#00h
	mov SG0TL,#90h
	mov SG1TH,#00h
	mov SG1TL,#0c0h
	ret

InitSgda:
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#00h
	mov IE0,#00h
	mov IE1,#00h
	mov IR0,#00h
	mov IR1,#00h
	mov SGC,#88h
	ei
	ret

PlaySgda:
	mov r3,#DAC_PHASE_PASSES
DacPass:
	mov r7,#DAC_SAMPLE_REPEATS
DacOuter:
	movw rr4,#0280h
	mov r1,#64
DacInner:
	mov r0,(rr4)+
	mov SGDA,r0
	call DelayDac
	dec r1
	br nz,DacInner
	dec r7
	br nz,DacOuter
	dec r3
	br nz,DacPass
	ret

SetupOverflowCommon:
	mov SGC,#00h
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#1fh
	mov SG1L,#1fh
	mov SG2L,#1fh
	mov SG0TH,#00h
	mov SG0TL,#20h
	mov SG1TH,#00h
	mov SG1TL,#21h
	mov SG2TH,#00h
	mov SG2TL,#20h
	mov SGDA,#80h
	mov SGC,#87h
	ret

PlayDacOverflowHi:
	movw rr4,#OverflowPosWave
	call SetupOverflowCommon
	call HoldPlay
	ret

PlayDacOverflowLo:
	movw rr4,#OverflowNegWave
	call SetupOverflowCommon
	call HoldPlay
	ret

MuteSound:
	di
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#00h
	mov SGC,#00h
	mov SGDA,#80h
	mov TM0C,#00h
	mov TM1C,#00h
	ret

SetupButtonScan:
	mov P0,#0ffh
	mov P1,#0ffh
	mov P2,#0ffh
	mov P0C,#55h
	mov P1C,#55h
	mov P2C,#95h
	ret

ReleaseButtonScan:
	mov P2,#0ffh
	mov P2C,#55h
	ret

ScanMainButtons:
	mov P2,#0ffh
	call DelayInput
	mov P2,#7fh
	call DelayInput
	ret

DelayInput:
	mov r6,#8
DelayInputLoop:
	dec r6
	br nz,DelayInputLoop
	ret

WaitButtonsReleased:
	call ScanMainButtons
	cmp P0,#0ffh
	br nz,WaitButtonsReleased
	cmp P1,#0ffh
	br nz,WaitButtonsReleased
	ret

WaitRestartButton:
	call SetupButtonScan
	call WaitButtonsReleased
WaitRestartPress:
	call ScanMainButtons
	cmp P0,#0ffh
	br nz,RestartButtonSeen
	cmp P1,#0ffh
	br nz,RestartButtonSeen
	br WaitRestartPress
RestartButtonSeen:
	call WaitButtonsReleased
	call ReleaseButtonScan
	ret
