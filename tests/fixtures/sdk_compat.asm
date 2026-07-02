RamHi equ 0364h -- SDK listings sometimes leave descriptive text here

Start
	mov RamHi,r0
	movw RamHi,rr2
	mov r3,RamHi
	movw rr2,RamHi
	movw rr2,#Start
	addw rr2,#Start
	mov mmu3,#1
	push dmpl
	push ps0
	push ps1
Text
	db 'A','B'
Pixels equ $-Text*8
	db 200-Pixels/2
