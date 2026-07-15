; Game.com DMA/LCDC visual hardware test.
; Build path: gamecom-as -> gamecom-romtool. No scripts or HDK data.bin.

	include "../common/gc_test_defs.inc"

	org 4000h

CurrentPage	equ 0d8h

BLine0		equ 0c050h
BLine1		equ 0c053h
BLine2		equ 0c056h
BLine3		equ 0c059h
BLine4		equ 0c05ch
BLine5		equ 0c05fh
BLine6		equ 0c062h
BLine7		equ 0c065h
BLine8		equ 0c068h
BLine9		equ 0c06bh
BLine10		equ 0c06eh
BLine12		equ 0c074h

	db 08h
	db 20h
	dw Entry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 00h
	db 00h
	dm 'DMA LCDC '
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
	call InitTestSystem
	clr r0
	ret

Main:
	call InitTestSystem
	call ClearVramBoth
	clr CurrentPage
	call RenderCurrentPage
	call SetupButtonScan
PageLoop:
	call WaitPagePress
	inc CurrentPage
	cmp CurrentPage,#0ah
	br ult,PageNoWrap
	clr CurrentPage
PageNoWrap:
	call RenderCurrentPage
	br PageLoop

RenderCurrentPage:
	call InitTestSystem
	call ClearVramBoth
	mov r0,CurrentPage
	cmp r0,#00h
	jmp eq,RenderIndex
	cmp r0,#01h
	jmp eq,RenderPalettePage
	cmp r0,#02h
	jmp eq,RenderCompoundPage
	cmp r0,#03h
	jmp eq,RenderPageCopyPage
	cmp r0,#04h
	jmp eq,RenderOverlapPage
	cmp r0,#05h
	jmp eq,RenderGrad80
	cmp r0,#06h
	jmp eq,RenderGrad90
	cmp r0,#07h
	jmp eq,RenderGradA0
	cmp r0,#08h
	jmp eq,RenderGradB0
	jmp RenderGeometryPage

RenderIndex:
	movw rr2,#Line0
	movw rr4,#StrIndexTitle
	call DrawString
	movw rr2,#Line2
	movw rr4,#StrIndex1
	call DrawString
	movw rr2,#Line3
	movw rr4,#StrIndex2
	call DrawString
	movw rr2,#Line4
	movw rr4,#StrIndex3
	call DrawString
	movw rr2,#Line5
	movw rr4,#StrIndex4
	call DrawString
	movw rr2,#Line6
	movw rr4,#StrIndex5
	call DrawString
	movw rr2,#Line7
	movw rr4,#StrIndex6
	call DrawString
	call DrawFooter
	ret

RenderPalettePage:
	call DrawDmaSourceA
	call DmaCopyIdentity
	call DmaCopyReversePalette
	movw rr2,#Line0
	movw rr4,#StrPalTitle
	call DrawString
	movw rr2,#Line1
	movw rr4,#StrPal1
	call DrawString
	movw rr2,#Line10
	movw rr4,#StrPal2
	call DrawString
	call DrawFooter
	ret

RenderCompoundPage:
	call DrawCompoundSourceA
	call FillCompoundDestA
	call DmaCopyCompound
	call DmaCopyOverwrite
	movw rr2,#Line0
	movw rr4,#StrCompTitle
	call DrawString
	movw rr2,#Line1
	movw rr4,#StrComp1
	call DrawString
	movw rr2,#Line10
	movw rr4,#StrComp2
	call DrawString
	call DrawFooter
	ret

RenderPageCopyPage:
	call DrawDmaSourceA
	call DmaCopyToPageB
	mov LCC,#0f0h
	movw rr2,#BLine0
	movw rr4,#StrPageTitle
	call DrawString
	movw rr2,#BLine1
	movw rr4,#StrPage1
	call DrawString
	movw rr2,#BLine10
	movw rr4,#StrPage2
	call DrawString
	movw rr2,#BLine12
	movw rr4,#StrFooter
	call DrawString
	ret

RenderOverlapPage:
	call DrawOverlapSourceA
	call DmaCopyOverlap
	call DmaCopyReverseX
	movw rr2,#Line0
	movw rr4,#StrOverTitle
	call DrawString
	movw rr2,#Line1
	movw rr4,#StrOver1
	call DrawString
	movw rr2,#Line10
	movw rr4,#StrOver2
	call DrawString
	call DrawFooter
	ret

RenderGrad80:
	mov LCC,#80h
	call DrawLcdcRamp
	movw rr2,#Line0
	movw rr4,#StrGrad80
	call DrawString
	call DrawFooter
	ret

RenderGrad90:
	mov LCC,#90h
	call DrawLcdcRamp
	movw rr2,#Line0
	movw rr4,#StrGrad90
	call DrawString
	call DrawFooter
	ret

RenderGradA0:
	mov LCC,#0a0h
	call DrawLcdcRamp
	movw rr2,#Line0
	movw rr4,#StrGradA0
	call DrawString
	call DrawFooter
	ret

RenderGradB0:
	mov LCC,#0b0h
	call DrawLcdcRamp
	movw rr2,#Line0
	movw rr4,#StrGradB0
	call DrawString
	call DrawFooter
	ret

RenderGeometryPage:
	mov LCH,#27h
	mov LCV,#37h
	mov LCC,#0b8h
	call DrawLcdcRamp
	movw rr2,#Line0
	movw rr4,#StrGeomTitle
	call DrawString
	movw rr2,#Line1
	movw rr4,#StrGeom1
	call DrawString
	movw rr2,#Line10
	movw rr4,#StrGeom2
	call DrawString
	call DrawFooter
	ret

DrawDmaSourceA:
	call BeginDirectVramWrite
	movw rr2,#VRAM_A
	mov r7,#16
DrawDmaSourceRows:
	mov r0,#0e4h
	mov (rr2)+,r0
	mov r0,#1bh
	mov (rr2)+,r0
	mov r0,#55h
	mov (rr2)+,r0
	mov r0,#0aah
	mov (rr2)+,r0
	addw rr2,#36
	dec r7
	br nz,DrawDmaSourceRows
	call EndDirectVramWrite
	ret

DrawCompoundSourceA:
	call BeginDirectVramWrite
	movw rr2,#VRAM_A
	mov r7,#16
DrawCompoundRows:
	mov r0,#0c0h
	mov (rr2)+,r0
	mov r0,#30h
	mov (rr2)+,r0
	mov r0,#0ch
	mov (rr2)+,r0
	mov r0,#03h
	mov (rr2)+,r0
	addw rr2,#36
	dec r7
	br nz,DrawCompoundRows
	call EndDirectVramWrite
	ret

FillCompoundDestA:
	call BeginDirectVramWrite
	movw rr2,#0a010h
	mov r7,#16
FillCompoundRows0:
	mov r6,#8
FillCompoundCols0:
	mov r0,#0ffh
	mov (rr2)+,r0
	dec r6
	br nz,FillCompoundCols0
	addw rr2,#32
	dec r7
	br nz,FillCompoundRows0
	movw rr2,#0a01ch
	mov r7,#16
FillCompoundRows1:
	mov r6,#8
FillCompoundCols1:
	mov r0,#0ffh
	mov (rr2)+,r0
	dec r6
	br nz,FillCompoundCols1
	addw rr2,#32
	dec r7
	br nz,FillCompoundRows1
	call EndDirectVramWrite
	ret

DrawOverlapSourceA:
	call BeginDirectVramWrite
	movw rr2,#0a780h
	mov r7,#18
DrawOverlapRows:
	mov r0,#0e4h
	mov (rr2)+,r0
	mov r0,#0e4h
	mov (rr2)+,r0
	mov r0,#00h
	mov (rr2)+,r0
	mov r0,#00h
	mov (rr2)+,r0
	addw rr2,#36
	dec r7
	br nz,DrawOverlapRows
	call EndDirectVramWrite
	ret

DrawLcdcRamp:
	call BeginDirectVramWrite
	movw rr2,#0a240h
	mov r7,#70
DrawRampLoop:
	mov r0,#00h
	mov (rr2)+,r0
	mov r0,#55h
	mov (rr2)+,r0
	mov r0,#0aah
	mov (rr2)+,r0
	mov r0,#0ffh
	mov (rr2)+,r0
	dec r7
	br nz,DrawRampLoop
	call EndDirectVramWrite
	movw rr2,#Line3
	movw rr4,#StrRamp1
	call DrawString
	movw rr2,#Line4
	movw rr4,#StrRamp2
	call DrawString
	movw rr2,#Line5
	movw rr4,#StrRamp3
	call DrawString
	movw rr2,#Line6
	movw rr4,#StrRamp4
	call DrawString
	ret

DmaCopyIdentity:
	mov DMX1,#00h
	mov DMY1,#00h
	mov DMX2,#40h
	mov DMY2,#00h
	mov DMDX,#1fh
	mov DMDY,#0fh
	mov DMPL,#0e4h
	mov DMVP,#00h
	mov DMC,#81h
	call DmaHaltWait
	ret

DmaCopyReversePalette:
	mov DMX1,#00h
	mov DMY1,#00h
	mov DMX2,#70h
	mov DMY2,#00h
	mov DMDX,#1fh
	mov DMDY,#0fh
	mov DMPL,#1bh
	mov DMVP,#00h
	mov DMC,#81h
	call DmaHaltWait
	ret

DmaCopyCompound:
	mov DMX1,#00h
	mov DMY1,#00h
	mov DMX2,#40h
	mov DMY2,#00h
	mov DMDX,#1fh
	mov DMDY,#0fh
	mov DMPL,#0e4h
	mov DMVP,#00h
	mov DMC,#80h
	call DmaHaltWait
	ret

DmaCopyOverwrite:
	mov DMX1,#00h
	mov DMY1,#00h
	mov DMX2,#70h
	mov DMY2,#00h
	mov DMDX,#1fh
	mov DMDY,#0fh
	mov DMPL,#0e4h
	mov DMVP,#00h
	mov DMC,#81h
	call DmaHaltWait
	ret

DmaCopyToPageB:
	mov DMX1,#00h
	mov DMY1,#00h
	mov DMX2,#40h
	mov DMY2,#00h
	mov DMDX,#1fh
	mov DMDY,#0fh
	mov DMPL,#0e4h
	mov DMVP,#02h
	mov DMC,#81h
	call DmaHaltWait
	ret

DmaCopyOverlap:
	mov DMX1,#00h
	mov DMY1,#30h
	mov DMX2,#04h
	mov DMY2,#30h
	mov DMDX,#3fh
	mov DMDY,#11h
	mov DMPL,#0e4h
	mov DMVP,#00h
	mov DMC,#81h
	call DmaHaltWait
	ret

DmaCopyReverseX:
	mov DMX1,#1fh
	mov DMY1,#30h
	mov DMX2,#70h
	mov DMY2,#30h
	mov DMDX,#1fh
	mov DMDY,#11h
	mov DMPL,#0e4h
	mov DMVP,#00h
	mov DMC,#89h
	call DmaHaltWait
	ret

StrIndexTitle:	dm 'DMA LCDC VISUAL TEST'
	db 0
StrIndex1:	dm '1 DMPL PAGE 0'
	db 0
StrIndex2:	dm '2 COMPOUND ZERO'
	db 0
StrIndex3:	dm '3 DMVP PAGE 1'
	db 0
StrIndex4:	dm '4 OVERLAP REVX'
	db 0
StrIndex5:	dm '5-8 LCC GRAD'
	db 0
StrIndex6:	dm '9 GEOM DIVIDER'
	db 0
StrPalTitle:	dm 'DMPL PALETTE'
	db 0
StrPal1:	dm 'SRC LEFT ID MID REV RIGHT'
	db 0
StrPal2:	dm 'DMPL E4 AND 1B'
	db 0
StrCompTitle:	dm 'COMPOUND ZERO'
	db 0
StrComp1:	dm 'MID COMPOUND RIGHT OVER'
	db 0
StrComp2:	dm 'ZERO SHOULD PRESERVE MID'
	db 0
StrPageTitle:	dm 'DMVP PAGE DEST'
	db 0
StrPage1:	dm 'DISPLAYING PAGE 1'
	db 0
StrPage2:	dm 'SOURCE WAS PAGE 0'
	db 0
StrOverTitle:	dm 'OVERLAP AND REVX'
	db 0
StrOver1:	dm 'SAME PAGE FEEDBACK'
	db 0
StrOver2:	dm 'RIGHT BLOCK USES REVX'
	db 0
StrGrad80:	dm 'LCC 80 GRAD 0'
	db 0
StrGrad90:	dm 'LCC 90 GRAD 1'
	db 0
StrGradA0:	dm 'LCC A0 GRAD 2'
	db 0
StrGradB0:	dm 'LCC B0 GRAD 3'
	db 0
StrGeomTitle:	dm 'LCDC GEOM DIV'
	db 0
StrGeom1:	dm 'LCH 27 LCV 37'
	db 0
StrGeom2:	dm 'LCC B8 DIV FIELD'
	db 0
StrRamp1:	dm 'RAMP 00 55 AA FF'
	db 0
StrRamp2:	dm 'WATCH SIZE SHADE'
	db 0
StrRamp3:	dm 'AND FRAME PACE'
	db 0
StrRamp4:	dm 'CAPTURE EACH PAGE'
	db 0

	include "../common/gc_test_common.inc"

	end
