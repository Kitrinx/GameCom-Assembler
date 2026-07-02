; WLA-style operand hints and bracketed memory spelling.

	org 0000h

SmallAddr	equ 0040h
ResNop		equ 0082h

	mov r0,SmallAddr.b
	mov r0,SmallAddr.w
	mov r0,ResNop
	mov r0,ResNop.b
	mov r2,#20h.b
	movw rr2,#1234h.w
	mov r1,[rr4]
	mov [rr4],r1
	mov [rr2]+,r0
	mov r0,[rr2]+
	bset #34h[r2],#3
	mov r10,#34h
	mov r11,r10
	movw rr10,#1234h

	end
