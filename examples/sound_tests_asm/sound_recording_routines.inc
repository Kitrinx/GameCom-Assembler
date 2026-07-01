; Recording/characterization suite for the ASM sound test ROM.

RecordingSuite:
	movw rr4,#SpansProgress01
	call RenderSpans
	call RecTestSg2Period
	movw rr4,#SpansProgress02
	call RenderSpans
	call RecTestSg2Level
	movw rr4,#SpansProgress03
	call RenderSpans
	call RecTestSgPeriod
	movw rr4,#SpansProgress04
	call RenderSpans
	call RecTestNibbleOrder
	movw rr4,#SpansProgress05
	call RenderSpans
	call RecTestLevelScaling
	movw rr4,#SpansProgress06
	call RenderSpans
	call RecTestMixerAddCancel
	movw rr4,#SpansProgress07
	call RenderSpans
	call RecTestAcOverflow
	movw rr4,#SpansProgress08
	call RenderSpans
	call RecTestDirectDacExclusive
	movw rr4,#SpansProgress09
	call RenderSpans
	call RecTestLiveRewrite
	movw rr4,#SpansProgress10
	call RenderSpans
	call RecTestDisableTail
	movw rr4,#SpansProgress11
	call RenderSpans
	call RecTestWaveWriteEnabled
	movw rr4,#SpansProgress12
	call RenderSpans
	call RecTestInvalidPeriods
	ret

HoldMarker:
	mov r6,#1
	call HoldCommon
	ret

HoldRec:
	mov r6,#REC_HOLD_UNITS
	call HoldCommon
	ret

HoldRecLong:
	mov r6,#REC_LONG_HOLD_UNITS
	call HoldCommon
	ret

MarkerGap:
	call MuteSound
	mov r6,#MARKER_GAP_UNITS
	call HoldCommon
	ret

AudioMarker:
	call MuteSound
	mov SGC,#88h
AudioMarkerLoop:
	mov SGDA,#0e0h
	call HoldMarker
	mov SGDA,#20h
	call HoldMarker
	mov SGDA,#80h
	call HoldMarker
	dec r7
	br nz,AudioMarkerLoop
	call MarkerGap
	ret

DelayDacMedium:
	mov r6,#80
DelayDacMediumLoop:
	dec r6
	br nz,DelayDacMediumLoop
	ret

PlayDacSquareRec:
	mov r7,#160
DacSquareRecLoop:
	mov SGDA,#0e0h
	call DelayDacMedium
	mov SGDA,#20h
	call DelayDacMedium
	dec r7
	br nz,DacSquareRecLoop
	mov SGDA,#80h
	ret

RecTestSg2Period:
	mov r7,#1
	call AudioMarker
	call SetupSg2_1f_001f
	call HoldRecLong
	call SetupSg2_1f_003f
	call HoldRecLong
	call SetupSg2_1f_007f
	call HoldRecLong
	call SetupSg2_1f_00ff
	call HoldRecLong
	call MarkerGap
	ret

RecTestSg2Level:
	mov r7,#2
	call AudioMarker
	call SetupSg2_01_003f
	call HoldRec
	call SetupSg2_04_003f
	call HoldRec
	call SetupSg2_08_003f
	call HoldRec
	call SetupSg2_10_003f
	call HoldRec
	call SetupSg2_1f_003f
	call HoldRec
	call MarkerGap
	ret

RecTestSgPeriod:
	mov r7,#3
	call AudioMarker
	call SetupSg0Alt_1f_0020_81
	call HoldRec
	call SetupSg0Alt_1f_0040_81
	call HoldRec
	call SetupSg0Alt_1f_0080_81
	call HoldRec
	call SetupSg1Alt_1f_0020_82
	call HoldRec
	call SetupSg1Alt_1f_0040_82
	call HoldRec
	call SetupSg1Alt_1f_0080_82
	call HoldRec
	call MarkerGap
	ret

RecTestNibbleOrder:
	mov r7,#4
	call AudioMarker
	call SetupSg0Nibble_1f_0100_81
	call HoldRecLong
	call MarkerGap
	ret

RecTestLevelScaling:
	mov r7,#5
	call AudioMarker
	call SetupSg0Alt_01_0040_81
	call HoldRec
	call SetupSg0Alt_04_0040_81
	call HoldRec
	call SetupSg0Alt_08_0040_81
	call HoldRec
	call SetupSg0Alt_10_0040_81
	call HoldRec
	call SetupSg0Alt_1f_0040_81
	call HoldRec
	call MarkerGap
	ret

RecTestMixerAddCancel:
	mov r7,#6
	call AudioMarker
	call SetupSg0Alt_10_0040_81
	call HoldRecLong
	call SetupSg0Sg1AltAlt_10_10_0040_0040_83_00_0000
	call HoldRecLong
	call SetupSg0Sg1AltInv_10_10_0040_0040_83_00_0000
	call HoldRecLong
	call MarkerGap
	ret

RecTestAcOverflow:
	mov r7,#7
	call AudioMarker
	call SetupSg0Sg1AltAlt_10_10_0040_0040_83_00_0000
	call HoldRec
	call SetupSg0Sg1AltAlt_1f_1f_0040_0040_83_00_0000
	call HoldRec
	call SetupSg0Sg1AltAlt_1f_1f_0040_0040_87_1f_0020
	call HoldRec
	call MarkerGap
	ret

RecTestDirectDacExclusive:
	mov r7,#8
	call AudioMarker
	call MuteSound
	mov SGC,#88h
	call PlayDacSquareRec
	call SetupSg0Alt_1f_0040_89
	call PlayDacSquareRec
	call SetupSg0Sg1AltInv_1f_1f_0040_0040_8f_1f_0020
	call PlayDacSquareRec
	call MarkerGap
	ret

RecTestLiveRewrite:
	mov r7,#9
	call AudioMarker
	call SetupSg0Alt_1f_0080_81
	call HoldRecLong
	mov SG0TH,#00h
	mov SG0TL,#20h
	call HoldRecLong
	mov SG0TH,#00h
	mov SG0TL,#0c0h
	call HoldRecLong
	call MarkerGap
	ret

RecTestDisableTail:
	mov r7,#10
	call AudioMarker
	call SetupSg0Alt_1f_0fff_81
	call HoldRec
	mov SGC,#00h
	call HoldRecLong
	call MarkerGap
	ret

RecTestWaveWriteEnabled:
	mov r7,#11
	call AudioMarker
	call SetupSg0Alt_1f_0040_81
	call HoldRecLong
	movw rr4,#ToneInvWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	call HoldRecLong
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	mov SGC,#81h
	call HoldRecLong
	call MarkerGap
	ret

RecTestInvalidPeriods:
	mov r7,#12
	call AudioMarker
	call SetupSg0Alt_1f_0000_81
	call HoldRec
	call SetupSg0Alt_1f_0001_81
	call HoldRec
	call SetupSg2_1f_0000
	call HoldRec
	call SetupSg2_1f_0001
	call HoldRec
	call MarkerGap
	ret

SetupSg2_1f_001f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#1fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_1f_003f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#3fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_1f_007f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#7fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_1f_00ff:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#0ffh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_01_003f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#01h
	mov SG2TH,#00h
	mov SG2TL,#3fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_04_003f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#04h
	mov SG2TH,#00h
	mov SG2TL,#3fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_08_003f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#08h
	mov SG2TH,#00h
	mov SG2TL,#3fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_10_003f:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#10h
	mov SG2TH,#00h
	mov SG2TL,#3fh
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_1f_0000:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg2_1f_0001:
	mov SGC,#00h
	mov SG0L,#00h
	mov SG1L,#00h
	mov SG2L,#1fh
	mov SG2TH,#00h
	mov SG2TL,#01h
	mov SGDA,#80h
	mov SGC,#84h
	ret

SetupSg0AltCore:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,r3
	mov SG1L,#00h
	mov SG2L,#00h
	mov SG0TH,r6
	mov SG0TL,r7
	mov SG1TH,#00h
	mov SG1TL,#00h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	ret

SetupSg1AltCore:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#00h
	mov SG1L,r3
	mov SG2L,#00h
	mov SG0TH,#00h
	mov SG0TL,#00h
	mov SG1TH,r6
	mov SG1TL,r7
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	ret

SetupSg0NibbleCore:
	mov SGC,#00h
	movw rr4,#NibbleProbeWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,r3
	mov SG1L,#00h
	mov SG2L,#00h
	mov SG0TH,r6
	mov SG0TL,r7
	mov SG1TH,#00h
	mov SG1TL,#00h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	ret

SetupSg0Alt_01_0040_81:
	mov r3,#01h
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_04_0040_81:
	mov r3,#04h
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_08_0040_81:
	mov r3,#08h
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_10_0040_81:
	mov r3,#10h
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0000_81:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#00h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0001_81:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#01h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0020_81:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#20h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0040_81:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0080_81:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#80h
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0fff_81:
	mov r3,#1fh
	mov r6,#0fh
	mov r7,#0ffh
	call SetupSg0AltCore
	mov SGC,#81h
	ret

SetupSg0Alt_1f_0040_89:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#40h
	call SetupSg0AltCore
	mov SGC,#89h
	ret

SetupSg0Nibble_1f_0100_81:
	mov r3,#1fh
	mov r6,#01h
	mov r7,#00h
	call SetupSg0NibbleCore
	mov SGC,#81h
	ret

SetupSg1Alt_1f_0020_82:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#20h
	call SetupSg1AltCore
	mov SGC,#82h
	ret

SetupSg1Alt_1f_0040_82:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#40h
	call SetupSg1AltCore
	mov SGC,#82h
	ret

SetupSg1Alt_1f_0080_82:
	mov r3,#1fh
	mov r6,#00h
	mov r7,#80h
	call SetupSg1AltCore
	mov SGC,#82h
	ret

SetupSg0Sg1AltAlt_10_10_0040_0040_83_00_0000:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#ToneAltWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#10h
	mov SG1L,#10h
	mov SG2L,#00h
	mov SG0TH,#00h
	mov SG0TL,#40h
	mov SG1TH,#00h
	mov SG1TL,#40h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	mov SGC,#83h
	ret

SetupSg0Sg1AltInv_10_10_0040_0040_83_00_0000:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#ToneInvWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#10h
	mov SG1L,#10h
	mov SG2L,#00h
	mov SG0TH,#00h
	mov SG0TL,#40h
	mov SG1TH,#00h
	mov SG1TL,#40h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	mov SGC,#83h
	ret

SetupSg0Sg1AltAlt_1f_1f_0040_0040_83_00_0000:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#ToneAltWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#1fh
	mov SG1L,#1fh
	mov SG2L,#00h
	mov SG0TH,#00h
	mov SG0TL,#40h
	mov SG1TH,#00h
	mov SG1TL,#40h
	mov SG2TH,#00h
	mov SG2TL,#00h
	mov SGDA,#80h
	mov SGC,#83h
	ret

SetupSg0Sg1AltAlt_1f_1f_0040_0040_87_1f_0020:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#ToneAltWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#1fh
	mov SG1L,#1fh
	mov SG2L,#1fh
	mov SG0TH,#00h
	mov SG0TL,#40h
	mov SG1TH,#00h
	mov SG1TL,#40h
	mov SG2TH,#00h
	mov SG2TL,#20h
	mov SGDA,#80h
	mov SGC,#87h
	ret

SetupSg0Sg1AltInv_1f_1f_0040_0040_8f_1f_0020:
	mov SGC,#00h
	movw rr4,#ToneAltWave
	mov r2,#60h
	mov r1,#16
	call Copy16ToSfr
	movw rr4,#ToneInvWave
	mov r2,#70h
	mov r1,#16
	call Copy16ToSfr
	mov SG0L,#1fh
	mov SG1L,#1fh
	mov SG2L,#1fh
	mov SG0TH,#00h
	mov SG0TL,#40h
	mov SG1TH,#00h
	mov SG1TL,#40h
	mov SG2TH,#00h
	mov SG2TL,#20h
	mov SGDA,#80h
	mov SGC,#8fh
	ret
