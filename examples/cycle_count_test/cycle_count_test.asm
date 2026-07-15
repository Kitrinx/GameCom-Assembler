; Game.com SM8521 cycle-count probe ROM.
; Build path: gamecom-as -> gamecom-romtool. No generator script or data.bin.

	org 4000h

IE0		equ 10h
IE1		equ 11h
IR0		equ 12h
IR1		equ 13h
P0		equ 14h
P1		equ 15h
P2		equ 16h
P3		equ 17h
SYS		equ 19h
CKC		equ 1ah
SPH		equ 1ch
SPL		equ 1dh
PS0		equ 1eh
PS1		equ 1fh
P0C		equ 20h
P1C		equ 21h
P2C		equ 22h
MMU2		equ 26h
MMU3		equ 27h
MMU4		equ 28h
LCC		equ 30h
LCH		equ 31h
LCV		equ 32h
DMC		equ 34h
TM0C		equ 50h
TM0D		equ 51h
WDTC		equ 5fh

TrialBaseline	equ 80h
TrialFirst	equ 81h
ResultPtr	equ 82h
DescPtr		equ 84h
OpcodeFlags	equ 86h
OpcodeLen	equ 87h
OpcodeByte	equ 88h
PageBase	equ 89h
RowOpcode	equ 8ah
TrialSecond	equ 8bh
RowLinePtr	equ 8ch
ClockStatus	equ 8dh
RowResultPtr	equ 8eh
Results		equ 90h
ResNop		equ Results+0
ResClrc		equ Results+1
ResComc		equ Results+2
ResSetc		equ Results+3
ResEi		equ Results+4
ResDi		equ Results+5
ResHalt		equ Results+6
ResStop		equ Results+7
ResClr		equ Results+8
ResNeg		equ Results+9
ResCom		equ Results+10
ResRr		equ Results+11
ResRl		equ Results+12
ResRrc		equ Results+13
ResRlc		equ Results+14
ResSrl		equ Results+15
ResSra		equ Results+16
ResSll		equ Results+17
ResInc		equ Results+18
ResDec		equ Results+19
ResDa		equ Results+20
ResSwap		equ Results+21
ResExts		equ Results+22
ResDmImm	equ Results+23
ResDmReg	equ Results+24
ResBtst		equ Results+25
ResCmp		equ Results+26
ResAdd		equ Results+27
ResSub		equ Results+28
ResAdc		equ Results+29
ResSbc		equ Results+30
ResAnd		equ Results+31
ResOr		equ Results+32
ResXor		equ Results+33
ResMov		equ Results+34
ResMovI		equ Results+35
ResMovInd	equ Results+36
ResMovW		equ Results+37
ResMovWI	equ Results+38
ResCmpW		equ Results+39
ResAddW		equ Results+40
ResSubW		equ Results+41
ResAdcW		equ Results+42
ResSbcW		equ Results+43
ResAndW		equ Results+44
ResOrW		equ Results+45
ResXorW		equ Results+46
ResIncW		equ Results+47
ResDecW		equ Results+48
ResBclr		equ Results+49
ResBset		equ Results+50
ResBbc		equ Results+51
ResBbs		equ Results+52
ResBmov		equ Results+53
ResBcmp		equ Results+54
ResBand		equ Results+55
ResBor		equ Results+56
ResBxor		equ Results+57
ResMult		equ Results+58
ResDiv		equ Results+59
ResMovm		equ Results+60
ResDbnz		equ Results+61
ResBr		equ Results+62
ResJmp		equ Results+63
ResCallRet	equ Results+64
ResRet		equ Results+65
ResIret		equ Results+66
ResPushPop	equ Results+67
ResPushWPopW	equ Results+68
ResInvalid	equ Results+69
ResReserved	equ Results+70
ResTimer	equ Results+71

CurrentPage	equ 0d8h
HexSave		equ 0d9h
SavedLcc	equ 0dah
RetDelta	equ 0dbh
RetCaptured	equ 0dch
RetMode		equ 0ddh
OpcodeTarget	equ 0deh
T0		equ 0e0h
T1		equ 0e1h
W0		equ 0e2h
W1		equ 0e4h
BaselineTarget	equ 0e6h
MemByte		equ 0e8h
DrawCount	equ 0e9h
LoopCounter	equ 0eah

Line0		equ 0a050h
Line1		equ 0a053h
Line2		equ 0a056h
Line3		equ 0a059h
Line4		equ 0a05ch
Line5		equ 0a05fh
Line6		equ 0a062h
Line7		equ 0a065h
Line8		equ 0a068h
Line9		equ 0a06bh
Line10		equ 0a06eh
Result2	equ 0ae66h
Result3	equ 0ae69h
Result4	equ 0ae6ch
Result5	equ 0ae6fh
Result6	equ 0ae72h
Result7	equ 0ae75h
Result8	equ 0ae78h
Result9	equ 0ae7bh
ResultTable	equ 0200h

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'CYC TEST '
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
	mov MMU2,#20h
	mov MMU3,#21h
	mov MMU4,#22h
	mov LCC,#0b0h
	mov LCH,#07h
	mov LCV,#27h
	call ClearVram
	movw rr2,#Line1
	movw rr4,#StrRunning
	call DrawString
	movw rr2,#Line2
	movw rr4,#StrProgress
	call DrawString
	call ValidateClock
	cmp ClockStatus,#00h
	br eq,ClockValid
	movw rr2,#Line3
	movw rr4,#StrClockError
	call DrawString
ClockErrorLoop:
	br ClockErrorLoop
ClockValid:
	call CalibrateRetDelta
	call RunTests
	clr CurrentPage
	call RenderCurrentPage
	call SetupButtonScan
PageLoop:
	call WaitPagePress
	inc CurrentPage
	cmp CurrentPage,#20h
	br nz,PageNoWrap
	clr CurrentPage
PageNoWrap:
	call RenderCurrentPage
	br PageLoop

RunTests:
	movw ResultPtr,#ResultTable
	movw DescPtr,#OpcodeDescriptors
	clr OpcodeByte
	movw BaselineTarget,#BaselineStub
RunOpcodeLoop:
	call DrawProgress
	call MeasureOpcode
	inc OpcodeByte
	br nz,RunOpcodeLoop
	call FinalizeControlResults
	ret
ValidateClock:
	clr ClockStatus
	mov r0,CKC
	and r0,#38h
	cmp r0,#20h
	br eq,ValidateClockDone
	mov ClockStatus,#01h
ValidateClockDone:
	ret

DrawProgress:
	movw rr2,#Result2
	mov r0,OpcodeByte
	call DrawHexByte
	ret

CalibrateRetDelta:
	call MeasureRetDeltaTrial
	mov TrialFirst,r0
	call MeasureRetDeltaTrial
	mov TrialSecond,r0
	cmp r0,TrialFirst
	br eq,CalibrateRetStableFirst
	call MeasureRetDeltaTrial
	cmp r0,TrialFirst
	br eq,CalibrateRetStableFirst
	cmp r0,TrialSecond
	br eq,CalibrateRetStableThird
	mov RetDelta,#0feh
	ret
CalibrateRetStableFirst:
	mov RetDelta,TrialFirst
	ret
CalibrateRetStableThird:
	mov RetDelta,TrialSecond
	ret

MeasureRetDeltaTrial:
	call MeasureRetBaselinePath
	mov TrialBaseline,r0
	call MeasureRetTestPath
	sub r0,TrialBaseline
	ret

MeasureRetBaselinePath:
	mov RetMode,#00h
	movw rr0,#RetCapture
	pushw rr0
	call StartTimer
	jmp RetBaselineStub

MeasureRetTestPath:
	mov RetMode,#01h
	movw rr0,#RetCapture
	pushw rr0
	call StartTimer
	jmp RetTestStub

RetBaselineStub:
	jmp RetCapture

RetTestStub:
	ret

RetCapture:
	mov r0,TM0D
	mov RetCaptured,r0
	cmp RetMode,#00h
	br nz,RetCaptureStackReady
	popw rr0
RetCaptureStackReady:
	mov r0,RetCaptured
	ret

FinalizeControlResults:
	cmp RetDelta,#0feh
	br nz,FinalizeRetStable
	mov r0,#0feh
	br FinalizeStoreRet
FinalizeRetStable:
	movw rr2,#ResultTable+098h
	mov r0,@rr2
	cmp r0,#0feh
	br eq,FinalizeRetUnstable
	cmp r0,#0ffh
	br eq,FinalizeRetUnstable
	add r0,RetDelta
	br FinalizeStoreRet
FinalizeRetUnstable:
	mov r0,#0feh
FinalizeStoreRet:
	mov RetCaptured,r0
	movw rr2,#ResultTable+0f8h
	mov @rr2,r0
	cmp r0,#0feh
	br eq,FinalizeCallsUnstable
	movw rr2,#ResultTable+03fh
	mov r1,@rr2
	sub r1,r0
	mov @rr2,r1
	movw rr2,#ResultTable+049h
	mov r1,@rr2
	sub r1,r0
	mov @rr2,r1
	ret
FinalizeCallsUnstable:
	mov r0,#0feh
	movw rr2,#ResultTable+03fh
	mov @rr2,r0
	movw rr2,#ResultTable+049h
	mov @rr2,r0
	ret

StartTimer:
	mov TM0C,#00h
	mov TM0D,#0ffh
	mov IR0,#00h
	mov TM0C,#80h
	ret

StoreAvg16:
StoreDelta1:
StoreResult:
	movw rr2,ResultPtr
	mov (rr2)+,r0
	movw ResultPtr,rr2
	ret

StoreSkip:
	mov r0,#0ffh
	movw rr2,ResultPtr
	mov (rr2)+,r0
	movw ResultPtr,rr2
	ret

MeasureOpcode:
	movw rr4,DescPtr
	mov r0,(rr4)+
	mov OpcodeFlags,r0
	mov r0,(rr4)+
	mov OpcodeLen,r0
	movw OpcodeTarget,rr4
	addw rr4,#0006h
	movw DescPtr,rr4
	cmp OpcodeLen,#00h
	br nz,OpcodeRunnable
	call StoreSkip
	ret
OpcodeRunnable:
	call MeasureOpcodeTrial
	mov TrialFirst,r0
	call MeasureOpcodeTrial
	mov TrialSecond,r0
	cmp r0,TrialFirst
	br eq,MeasureOpcodeStableFirst
	call MeasureOpcodeTrial
	cmp r0,TrialFirst
	br eq,MeasureOpcodeStableFirst
	cmp r0,TrialSecond
	br eq,MeasureOpcodeStableThird
	mov r0,#0feh
	br MeasureOpcodeStore
MeasureOpcodeStableFirst:
	mov r0,TrialFirst
	br MeasureOpcodeStore
MeasureOpcodeStableThird:
	mov r0,TrialSecond
MeasureOpcodeStore:
	call StoreResult
	ret

MeasureOpcodeTrial:
	call PrepareOpcodeState
	call StartTimer
	call @BaselineTarget
	mov r0,TM0D
	mov TrialBaseline,r0
	call PrepareOpcodeState
	call StartTimer
	call @OpcodeTarget
	mov r0,TM0D
	di
	mov IE0,#00h
	mov IE1,#00h
	sub r0,TrialBaseline
	ret
PrepareOpcodeState:
	mov T0,#55h
	mov T1,#33h
	mov MemByte,#5ah
	mov DrawCount,#0a5h
	movw W0,#1234h
	movw W1,#0003h
	mov r0,#0e0h
	mov r1,#05h
	mov r2,#0e0h
	mov r3,#00h
	mov r4,#00h
	mov r5,#00h
	mov r6,#00h
	mov r7,#00h
	movw rr4,#1234h
	movw rr6,#MemByte
	mov r0,OpcodeFlags
	and r0,#01h
	br z,PrepNoR4Byte
	mov r4,#MemByte
PrepNoR4Byte:
	mov r0,OpcodeFlags
	and r0,#02h
	br z,PrepNoRr4Mem
	movw rr4,#MemByte
PrepNoRr4Mem:
	mov r0,OpcodeFlags
	and r0,#04h
	br z,PrepNoRr4Target
	movw rr4,#StaticStubRet
PrepNoRr4Target:
	mov r0,OpcodeFlags
	and r0,#08h
	br z,PrepNoRr6Mem
	movw rr6,#MemByte
PrepNoRr6Mem:
	mov r0,OpcodeFlags
	and r0,#10h
	br z,PrepNoDivPair
	movw rr4,#1234h
	movw rr6,#0003h
PrepNoDivPair:
	; Use one deterministic condition state and restore the sandbox pointer.
	mov PS1,#00h
	mov r0,#0e0h
	ret

TestNop:
	call StartTimer
	mov r7,#16
TestNopLoop:
	nop
	dec r7
	br nz,TestNopLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestClrc:
	call StartTimer
	mov r7,#16
TestClrcLoop:
	clrc
	dec r7
	br nz,TestClrcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestComc:
	call StartTimer
	mov r7,#16
TestComcLoop:
	comc
	dec r7
	br nz,TestComcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSetc:
	call StartTimer
	mov r7,#16
TestSetcLoop:
	setc
	dec r7
	br nz,TestSetcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestEi:
	call StartTimer
	mov r7,#16
TestEiLoop:
	ei
	dec r7
	br nz,TestEiLoop
	mov r0,TM0D
	call StoreAvg16
	di
	ret

TestDi:
	call StartTimer
	mov r7,#16
TestDiLoop:
	di
	dec r7
	br nz,TestDiLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestClr:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestClrLoop:
	clr T0
	dec r7
	br nz,TestClrLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestNeg:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestNegLoop:
	neg T0
	dec r7
	br nz,TestNegLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestCom:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestComLoop:
	com T0
	dec r7
	br nz,TestComLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestRr:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestRrLoop:
	rr T0
	dec r7
	br nz,TestRrLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestRl:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestRlLoop:
	rl T0
	dec r7
	br nz,TestRlLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestRrc:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestRrcLoop:
	rrc T0
	dec r7
	br nz,TestRrcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestRlc:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestRlcLoop:
	rlc T0
	dec r7
	br nz,TestRlcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSrl:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestSrlLoop:
	srl T0
	dec r7
	br nz,TestSrlLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSra:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestSraLoop:
	sra T0
	dec r7
	br nz,TestSraLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSll:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestSllLoop:
	sll T0
	dec r7
	br nz,TestSllLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestInc:
	mov T0,#00h
	call StartTimer
	mov r7,#16
TestIncLoop:
	inc T0
	dec r7
	br nz,TestIncLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDec:
	mov T0,#00h
	call StartTimer
	mov r7,#16
TestDecLoop:
	dec T0
	dec r7
	br nz,TestDecLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDa:
	mov T0,#45h
	call StartTimer
	mov r7,#16
TestDaLoop:
	da T0
	dec r7
	br nz,TestDaLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSwap:
	mov T0,#5ah
	call StartTimer
	mov r7,#16
TestSwapLoop:
	swap T0
	dec r7
	br nz,TestSwapLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestExts:
	mov T0,#80h
	call StartTimer
	mov r7,#16
TestExtsLoop:
	exts T0
	dec r7
	br nz,TestExtsLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDmImm:
	call StartTimer
	mov r7,#16
TestDmImmLoop:
	dm #12h
	dec r7
	br nz,TestDmImmLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDmReg:
	mov T0,#12h
	call StartTimer
	mov r7,#16
TestDmRegLoop:
	dm T0
	dec r7
	br nz,TestDmRegLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBtst:
	mov T0,#08h
	call StartTimer
	mov r7,#16
TestBtstLoop:
	btst T0,#08h
	dec r7
	br nz,TestBtstLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestCmp:
	mov T0,#22h
	mov T1,#11h
	call StartTimer
	mov r7,#16
TestCmpLoop:
	cmp T0,T1
	dec r7
	br nz,TestCmpLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAdd:
	mov T0,#01h
	mov T1,#01h
	call StartTimer
	mov r7,#16
TestAddLoop:
	add T0,T1
	dec r7
	br nz,TestAddLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSub:
	mov T0,#80h
	mov T1,#01h
	call StartTimer
	mov r7,#16
TestSubLoop:
	sub T0,T1
	dec r7
	br nz,TestSubLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAdc:
	mov T0,#01h
	mov T1,#01h
	clrc
	call StartTimer
	mov r7,#16
TestAdcLoop:
	adc T0,T1
	dec r7
	br nz,TestAdcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSbc:
	mov T0,#80h
	mov T1,#01h
	clrc
	call StartTimer
	mov r7,#16
TestSbcLoop:
	sbc T0,T1
	dec r7
	br nz,TestSbcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAnd:
	mov T0,#0f0h
	mov T1,#55h
	call StartTimer
	mov r7,#16
TestAndLoop:
	and T0,T1
	dec r7
	br nz,TestAndLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestOr:
	mov T0,#0f0h
	mov T1,#55h
	call StartTimer
	mov r7,#16
TestOrLoop:
	or T0,T1
	dec r7
	br nz,TestOrLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestXor:
	mov T0,#0f0h
	mov T1,#55h
	call StartTimer
	mov r7,#16
TestXorLoop:
	xor T0,T1
	dec r7
	br nz,TestXorLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMov:
	mov T1,#55h
	call StartTimer
	mov r7,#16
TestMovLoop:
	mov T0,T1
	dec r7
	br nz,TestMovLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMovI:
	call StartTimer
	mov r7,#16
TestMovILoop:
	mov T0,#55h
	dec r7
	br nz,TestMovILoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMovInd:
	mov MemByte,#5ah
	movw rr4,#MemByte
	call StartTimer
	mov r7,#16
TestMovIndLoop:
	mov r0,@rr4
	dec r7
	br nz,TestMovIndLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMovW:
	movw W1,#1234h
	call StartTimer
	mov r7,#16
TestMovWLoop:
	movw W0,W1
	dec r7
	br nz,TestMovWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMovWI:
	call StartTimer
	mov r7,#16
TestMovWILoop:
	movw W0,#1234h
	dec r7
	br nz,TestMovWILoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestCmpW:
	movw W0,#1234h
	movw W1,#0010h
	call StartTimer
	mov r7,#16
TestCmpWLoop:
	cmpw W0,W1
	dec r7
	br nz,TestCmpWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAddW:
	movw W0,#0001h
	movw W1,#0001h
	call StartTimer
	mov r7,#16
TestAddWLoop:
	addw W0,W1
	dec r7
	br nz,TestAddWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSubW:
	movw W0,#1000h
	movw W1,#0001h
	call StartTimer
	mov r7,#16
TestSubWLoop:
	subw W0,W1
	dec r7
	br nz,TestSubWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAdcW:
	movw W0,#0001h
	movw W1,#0001h
	clrc
	call StartTimer
	mov r7,#16
TestAdcWLoop:
	adcw W0,W1
	dec r7
	br nz,TestAdcWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestSbcW:
	movw W0,#1000h
	movw W1,#0001h
	clrc
	call StartTimer
	mov r7,#16
TestSbcWLoop:
	sbcw W0,W1
	dec r7
	br nz,TestSbcWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestAndW:
	movw W0,#0f0fh
	movw W1,#00ffh
	call StartTimer
	mov r7,#16
TestAndWLoop:
	andw W0,W1
	dec r7
	br nz,TestAndWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestOrW:
	movw W0,#0f0fh
	movw W1,#00ffh
	call StartTimer
	mov r7,#16
TestOrWLoop:
	orw W0,W1
	dec r7
	br nz,TestOrWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestXorW:
	movw W0,#0f0fh
	movw W1,#00ffh
	call StartTimer
	mov r7,#16
TestXorWLoop:
	xorw W0,W1
	dec r7
	br nz,TestXorWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestIncW:
	movw W0,#0000h
	call StartTimer
	mov r7,#16
TestIncWLoop:
	incw W0
	dec r7
	br nz,TestIncWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDecW:
	movw W0,#0000h
	call StartTimer
	mov r7,#16
TestDecWLoop:
	decw W0
	dec r7
	br nz,TestDecWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBclr:
	mov T0,#0ffh
	call StartTimer
	mov r7,#16
TestBclrLoop:
	bclr T0,#0
	dec r7
	br nz,TestBclrLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBset:
	mov T0,#00h
	call StartTimer
	mov r7,#16
TestBsetLoop:
	bset T0,#0
	dec r7
	br nz,TestBsetLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBbc:
	mov T0,#00h
	call StartTimer
	mov r7,#16
TestBbcLoop:
	bbc T0,#0,TestBbcAfter
TestBbcAfter:
	dec r7
	br nz,TestBbcLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBbs:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBbsLoop:
	bbs T0,#0,TestBbsAfter
TestBbsAfter:
	dec r7
	br nz,TestBbsLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBmov:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBmovLoop:
	bmov bf,T0,#0
	dec r7
	br nz,TestBmovLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBcmp:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBcmpLoop:
	bcmp bf,T0,#0
	dec r7
	br nz,TestBcmpLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBand:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBandLoop:
	band bf,T0,#0
	dec r7
	br nz,TestBandLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBor:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBorLoop:
	bor T0,#0,bf
	dec r7
	br nz,TestBorLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBxor:
	mov T0,#01h
	call StartTimer
	mov r7,#16
TestBxorLoop:
	bxor T0,#0,bf
	dec r7
	br nz,TestBxorLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestMult:
	mov T0,#03h
	mov T1,#05h
	call StartTimer
	mov r7,#1
TestMultLoop:
	mult T0,T1
	dec r7
	br nz,TestMultLoop
	mov r0,TM0D
	call StoreDelta1
	ret

TestDiv:
	movw W0,#1234h
	call StartTimer
	mov r7,#1
TestDivLoop:
	div W0,#03h
	dec r7
	br nz,TestDivLoop
	mov r0,TM0D
	call StoreDelta1
	ret

TestMovm:
	call StartTimer
	mov r7,#16
TestMovmLoop:
	movm T0,#0fh,#55h
	dec r7
	br nz,TestMovmLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestDbnz:
	mov r6,#16
	call StartTimer
	mov r7,#16
TestDbnzLoop:
	dbnz r6,TestDbnzAfter
TestDbnzAfter:
	dec r7
	br nz,TestDbnzLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestBr:
	call StartTimer
	mov r7,#16
TestBrLoop:
	br TestBrAfter
TestBrAfter:
	dec r7
	br nz,TestBrLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestJmp:
	call StartTimer
	mov r7,#16
TestJmpLoop:
	jmp TestJmpAfter
TestJmpAfter:
	dec r7
	br nz,TestJmpLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestCallRet:
	call StartTimer
	mov r7,#16
TestCallRetLoop:
	call EmptyRet
	dec r7
	br nz,TestCallRetLoop
	mov r0,TM0D
	call StoreAvg16
	ret

EmptyRet:
	ret

TestPushPop:
	mov T0,#55h
	call StartTimer
	mov r7,#16
TestPushPopLoop:
	push T0
	pop T0
	dec r7
	br nz,TestPushPopLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestPushWPopW:
	movw W0,#1234h
	call StartTimer
	mov r7,#16
TestPushWPopWLoop:
	pushw W0
	popw W0
	dec r7
	br nz,TestPushWPopWLoop
	mov r0,TM0D
	call StoreAvg16
	ret

TestTimer:
	call StartTimer
	mov r7,#16
TestTimerLoop:
	mov r0,TM0D
	dec r7
	br nz,TestTimerLoop
	mov r0,TM0D
	call StoreAvg16
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

DrawString:
	mov r0,(rr4)+
	cmp r0,#00h
	br eq,DrawStringDone
	call DrawChar
	br DrawString
DrawStringDone:
	ret

DrawChar:
	call BeginDirectVramWrite
	sub r0,#20h
	movw rr6,#FontTable
DrawCharSeek:
	cmp r0,#00h
	br eq,DrawCharGlyph
	addw rr6,#10
	dec r0
	br DrawCharSeek
DrawCharGlyph:
	mov DrawCount,#5
DrawCharColumn:
	mov r1,(rr6)+
	mov @rr2,r1
	mov r1,(rr6)+
	mov 1(rr2),r1
	addw rr2,#40
	dec DrawCount
	br nz,DrawCharColumn
	clr r1
	mov @rr2,r1
	mov 1(rr2),r1
	addw rr2,#40
	call EndDirectVramWrite
	ret

BeginDirectVramWrite:
	mov SavedLcc,LCC
	bclr LCC,#7
	ret

EndDirectVramWrite:
	mov LCC,SavedLcc
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
	mov HexSave,r0
	srl r0
	srl r0
	srl r0
	srl r0
	call DrawHexNibble
	mov r0,HexSave
	call DrawHexNibble
	ret

DrawResultByte:
	mov HexSave,r0
	cmp r0,#0ffh
	br nz,DrawResultCheckUnstable
	movw rr4,#StrSkip
	call DrawString
	ret
DrawResultCheckUnstable:
	cmp r0,#0feh
	br nz,DrawResultHex
	movw rr4,#StrUnstable
	call DrawString
	ret
DrawResultHex:
	mov r0,HexSave
	call DrawHexByte
	ret

DrawRow2:
	mov RowOpcode,r0
	movw RowLinePtr,#Line2
	movw RowResultPtr,#Result2
	call DrawOpcodeRow
	ret

DrawRow3:
	mov RowOpcode,r0
	movw RowLinePtr,#Line3
	movw RowResultPtr,#Result3
	call DrawOpcodeRow
	ret

DrawRow4:
	mov RowOpcode,r0
	movw RowLinePtr,#Line4
	movw RowResultPtr,#Result4
	call DrawOpcodeRow
	ret

DrawRow5:
	mov RowOpcode,r0
	movw RowLinePtr,#Line5
	movw RowResultPtr,#Result5
	call DrawOpcodeRow
	ret

DrawRow6:
	mov RowOpcode,r0
	movw RowLinePtr,#Line6
	movw RowResultPtr,#Result6
	call DrawOpcodeRow
	ret

DrawRow7:
	mov RowOpcode,r0
	movw RowLinePtr,#Line7
	movw RowResultPtr,#Result7
	call DrawOpcodeRow
	ret

DrawRow8:
	mov RowOpcode,r0
	movw RowLinePtr,#Line8
	movw RowResultPtr,#Result8
	call DrawOpcodeRow
	ret

DrawRow9:
	mov RowOpcode,r0
	movw RowLinePtr,#Line9
	movw RowResultPtr,#Result9
	call DrawOpcodeRow
	ret

DrawOpcodeRow:
	movw rr2,RowLinePtr
	mov r0,RowOpcode
	call DrawHexByte
	mov r0,#20h
	call DrawChar
	movw rr6,#OpcodeNamePtrs
	mov r1,RowOpcode
DrawNameSeek:
	cmp r1,#00h
	br eq,DrawNameFound
	addw rr6,#2
	dec r1
	br DrawNameSeek
DrawNameFound:
	movw rr4,(rr6)+
	call DrawString
	movw rr2,RowResultPtr
	movw rr6,#ResultTable
	mov r1,RowOpcode
DrawResultSeek:
	cmp r1,#00h
	br eq,DrawResultFound
	incw rr6
	dec r1
	br DrawResultSeek
DrawResultFound:
	mov r0,@rr6
	call DrawResultByte
	ret

DrawHeader:
	call ClearVram
	movw rr2,#Line0
	movw rr4,#StrTitle
	call DrawString
	movw rr2,#Line1
	ret

RenderCurrentPage:
	call DrawHeader
	call ComputePageBase
	movw rr4,#StrOpRange
	call DrawString
	mov r0,PageBase
	call DrawHexByte
	mov r0,#2dh
	call DrawChar
	mov r0,PageBase
	add r0,#07h
	call DrawHexByte
	mov r0,PageBase
	call DrawRow2
	mov r0,PageBase
	add r0,#01h
	call DrawRow3
	mov r0,PageBase
	add r0,#02h
	call DrawRow4
	mov r0,PageBase
	add r0,#03h
	call DrawRow5
	mov r0,PageBase
	add r0,#04h
	call DrawRow6
	mov r0,PageBase
	add r0,#05h
	call DrawRow7
	mov r0,PageBase
	add r0,#06h
	call DrawRow8
	mov r0,PageBase
	add r0,#07h
	call DrawRow9
	ret

ComputePageBase:
	clr PageBase
	mov r1,CurrentPage
ComputePageBaseLoop:
	cmp r1,#00h
	br eq,ComputePageBaseDone
	add PageBase,#08h
	dec r1
	br ComputePageBaseLoop
ComputePageBaseDone:
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

StrRunning:	dm 'RUNNING CYCLE TEST'
	db 0
StrProgress:	dm 'OPCODE'
	db 0
StrClockError:	dm 'CLOCK IS NOT FCK/2'
	db 0
StrTitle:	dm 'SM8521 OPCODE CYCLES'
	db 0
StrOpRange:	dm 'OP '
	db 0
StrSkip:	dm '--'
	db 0
StrUnstable:	dm '??'
	db 0

BaselineStub:
StaticStubRet:
	ret

	include "opcode_tables.inc"

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

	end
