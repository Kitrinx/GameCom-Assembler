; Game.com hello-world cartridge.
; Build path is intentionally plain: assemble this file, then sign the raw
; output with gamecom-romtool. No generated source and no HDK data.bin filler.

	org 4000h

VRAM_A		equ 0a000h
VRAM_A_END	equ 0bf3fh
HELLO_DST	equ 0a4c0h	; x=30, y=64 in the rotated VRAM layout
FONT_DST	equ 0bd60h	; small raw test.chr swatch at the right edge

	db 08h
	db 20h
	dw GameEntry
	db 00000011b
	dm 'TigerDMGC'
	db 00h
	db 0
	db 0
	dm 'HELLOWRLD'
	dw 1a18h
	db 0
	db 0
	dw 0

GameEntry:
	clr r0
	cmp r2,#0
	br eq,EntryReturn
	cmp r2,#1
	jmp eq,GameMain
	cmp r2,#2
	br eq,EntryReturn
	cmp r2,#3
	br eq,EntryReturn
	mov r0,#0ffh
EntryReturn:
	ret

GameMain:
	di
	mov lcc,#0b0h
	mov lch,#07h
	mov lcv,#27h
	call ClearVramA
	movw rr2,#HELLO_DST
	movw rr4,#HelloMessage
	call DrawGlyphMessage
	call DrawTestChrSwatch
Forever:
	br Forever

ClearVramA:
	movw rr2,#VRAM_A
	clr r0
ClearVramLoop:
	mov (rr2)+,r0
	cmpw rr2,#VRAM_A_END
	br ule,ClearVramLoop
	ret

DrawGlyphMessage:
	movw rr6,(rr4)+
	cmpw rr6,#0
	br eq,DrawGlyphDone
	mov r1,#5
DrawGlyphColumn:
	mov r0,(rr6)+
	mov @rr2,r0
	mov r0,(rr6)+
	mov 1(rr2),r0
	addw rr2,#40
	dec r1
	br nz,DrawGlyphColumn
	clr r0
	mov @rr2,r0
	mov 1(rr2),r0
	addw rr2,#80
	br DrawGlyphMessage
DrawGlyphDone:
	ret

DrawTestChrSwatch:
	movw rr4,#TestChr
	movw rr2,#FONT_DST
	movw rr6,#320
DrawTestChrLoop:
	mov r0,(rr4)+
	mov (rr2)+,r0
	decw rr6
	br nz,DrawTestChrLoop
	ret

HelloMessage:
	dw GlyphH
	dw GlyphE
	dw GlyphL
	dw GlyphL
	dw GlyphO
	dw GlyphSpace
	dw GlyphW
	dw GlyphO
	dw GlyphR
	dw GlyphL
	dw GlyphD
	dw 0

GlyphH:
	db 0ffh,0fch, 03h,00h, 03h,00h, 03h,00h, 0ffh,0fch
GlyphE:
	db 0ffh,0fch, 0c3h,0ch, 0c3h,0ch, 0c3h,0ch, 0c0h,0ch
GlyphL:
	db 0ffh,0fch, 00h,0ch, 00h,0ch, 00h,0ch, 00h,0ch
GlyphO:
	db 03fh,0f0h, 0c0h,0ch, 0c0h,0ch, 0c0h,0ch, 03fh,0f0h
GlyphSpace:
	db 00h,00h, 00h,00h, 00h,00h, 00h,00h, 00h,00h
GlyphW:
	db 0ffh,0fch, 00h,30h, 03h,0c0h, 00h,30h, 0ffh,0fch
GlyphR:
	db 0ffh,0fch, 0c3h,00h, 0c3h,0c0h, 0c3h,30h, 03ch,0ch
GlyphD:
	db 0ffh,0fch, 0c0h,0ch, 0c0h,0ch, 0c0h,0ch, 03fh,0f0h

TestChr:
	incbin "test.chr"

	end
