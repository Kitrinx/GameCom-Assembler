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
FlagOn equ 1
FlagOff equ 0
IF FlagOff
	db 0eeh
ENDIF
IF FlagOn
	db 0aah
ENDIF
IF FlagOff
	db 0eeh
ELSE
	db 055h
ENDIF
IF .NOT. FlagOff
	db 066h
ENDIF
IF 2 .GT. 1
	db 077h
ENDIF
