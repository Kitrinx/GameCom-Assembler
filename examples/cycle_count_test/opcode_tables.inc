; ROM-resident opcode descriptor/name table for cycle_count_test.asm.
; Fixed descriptor entry: flags, length, six stub bytes.

OpcodeDescriptors:
; 00-07
	db 00h,02h,00h,0e0h,0f8h,00h,00h,00h	; 00 CLR DIR
	db 00h,02h,01h,0e0h,0f8h,00h,00h,00h	; 01 NEG DIR
	db 00h,02h,02h,0e0h,0f8h,00h,00h,00h	; 02 COM DIR
	db 00h,02h,03h,0e0h,0f8h,00h,00h,00h	; 03 RR DIR
	db 00h,02h,04h,0e0h,0f8h,00h,00h,00h	; 04 RL DIR
	db 00h,02h,05h,0e0h,0f8h,00h,00h,00h	; 05 RRC DIR
	db 00h,02h,06h,0e0h,0f8h,00h,00h,00h	; 06 RLC DIR
	db 00h,02h,07h,0e0h,0f8h,00h,00h,00h	; 07 SRL DIR
; 08-0F
	db 00h,02h,08h,0e0h,0f8h,00h,00h,00h	; 08 INC DIR
	db 00h,02h,09h,0e0h,0f8h,00h,00h,00h	; 09 DEC DIR
	db 00h,02h,0ah,0e0h,0f8h,00h,00h,00h	; 0A SRA DIR
	db 00h,02h,0bh,0e0h,0f8h,00h,00h,00h	; 0B SLL DIR
	db 00h,02h,0ch,0e0h,0f8h,00h,00h,00h	; 0C DA DIR
	db 00h,02h,0dh,0e0h,0f8h,00h,00h,00h	; 0D SWAP DIR
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 0E PUSH D
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 0F POP D
; 10-17
	db 00h,02h,10h,01h,0f8h,00h,00h,00h	; 10 CMP R,R
	db 00h,02h,11h,01h,0f8h,00h,00h,00h	; 11 ADD R,R
	db 00h,02h,12h,01h,0f8h,00h,00h,00h	; 12 SUB R,R
	db 00h,02h,13h,01h,0f8h,00h,00h,00h	; 13 ADC R,R
	db 00h,02h,14h,01h,0f8h,00h,00h,00h	; 14 SBC R,R
	db 00h,02h,15h,01h,0f8h,00h,00h,00h	; 15 AND R,R
	db 00h,02h,16h,01h,0f8h,00h,00h,00h	; 16 OR R,R
	db 00h,02h,17h,01h,0f8h,00h,00h,00h	; 17 XOR R,R
; 18-1F
	db 00h,02h,18h,0e2h,0f8h,00h,00h,00h	; 18 INCW D
	db 00h,02h,19h,0e2h,0f8h,00h,00h,00h	; 19 DECW D
	db 01h,02h,1ah,20h,0f8h,00h,00h,00h	; 1A UNARY @R
	db 01h,00h,00h,00h,00h,00h,00h,00h	; 1B STK @R
	db 00h,03h,1ch,13h,00h,0f8h,00h,00h	; 1C BCLR IX
	db 00h,03h,1dh,13h,00h,0f8h,00h,00h	; 1D BSET IX
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 1E PUSHW D
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 1F POPW D
; 20-27
	db 01h,02h,20h,04h,0f8h,00h,00h,00h	; 20 CMP R,@R
	db 01h,02h,21h,04h,0f8h,00h,00h,00h	; 21 ADD R,@R
	db 01h,02h,22h,04h,0f8h,00h,00h,00h	; 22 SUB R,@R
	db 01h,02h,23h,04h,0f8h,00h,00h,00h	; 23 ADC R,@R
	db 01h,02h,24h,04h,0f8h,00h,00h,00h	; 24 SBC R,@R
	db 01h,02h,25h,04h,0f8h,00h,00h,00h	; 25 AND R,@R
	db 01h,02h,26h,04h,0f8h,00h,00h,00h	; 26 OR R,@R
	db 01h,02h,27h,04h,0f8h,00h,00h,00h	; 27 XOR R,@R
; 28-2F
	db 01h,02h,28h,04h,0f8h,00h,00h,00h	; 28 MOV R,@R
	db 01h,02h,29h,04h,0f8h,00h,00h,00h	; 29 MOV @R,R
	db 00h,04h,2ah,10h,00h,00h,0f8h,00h	; 2A BBC IX
	db 00h,04h,2bh,10h,00h,00h,0f8h,00h	; 2B BBS IX
	db 00h,02h,2ch,0e0h,0f8h,00h,00h,00h	; 2C EXTS D
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 2D DM #
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 2E MOV PS0
	db 00h,03h,2fh,0e0h,08h,0f8h,00h,00h	; 2F BTST D
; 30-37
	db 02h,02h,30h,04h,0f8h,00h,00h,00h	; 30 CMP R,@RR
	db 02h,02h,31h,04h,0f8h,00h,00h,00h	; 31 ADD R,@RR
	db 02h,02h,32h,04h,0f8h,00h,00h,00h	; 32 SUB R,@RR
	db 02h,02h,33h,04h,0f8h,00h,00h,00h	; 33 ADC R,@RR
	db 02h,02h,34h,04h,0f8h,00h,00h,00h	; 34 SBC R,@RR
	db 02h,02h,35h,04h,0f8h,00h,00h,00h	; 35 AND R,@RR
	db 02h,02h,36h,04h,0f8h,00h,00h,00h	; 36 OR R,@RR
	db 02h,02h,37h,04h,0f8h,00h,00h,00h	; 37 XOR R,@RR
; 38-3F
	db 02h,02h,38h,04h,0f8h,00h,00h,00h	; 38 MOV R,@RR
	db 02h,02h,39h,04h,0f8h,00h,00h,00h	; 39 MOV @RR,R
	db 08h,02h,3ah,06h,0f8h,00h,00h,00h	; 3A MOVW R,@
	db 08h,02h,3bh,06h,0f8h,00h,00h,00h	; 3B MOVW @,R
	db 00h,02h,3ch,26h,0f8h,00h,00h,00h	; 3C MOVW R,R
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 3D DM D
	db 04h,02h,3eh,04h,0f8h,00h,00h,00h	; 3E JMP RR
	db 04h,02h,3fh,04h,0f8h,00h,00h,00h	; 3F CALL RR
; 40-47
	db 00h,03h,40h,0e1h,0e0h,0f8h,00h,00h	; 40 CMP D,D
	db 00h,03h,41h,0e1h,0e0h,0f8h,00h,00h	; 41 ADD D,D
	db 00h,03h,42h,0e1h,0e0h,0f8h,00h,00h	; 42 SUB D,D
	db 00h,03h,43h,0e1h,0e0h,0f8h,00h,00h	; 43 ADC D,D
	db 00h,03h,44h,0e1h,0e0h,0f8h,00h,00h	; 44 SBC D,D
	db 00h,03h,45h,0e1h,0e0h,0f8h,00h,00h	; 45 AND D,D
	db 00h,03h,46h,0e1h,0e0h,0f8h,00h,00h	; 46 OR D,D
	db 00h,03h,47h,0e1h,0e0h,0f8h,00h,00h	; 47 XOR D,D
; 48-4F
	db 00h,03h,48h,0e1h,0e0h,0f8h,00h,00h	; 48 MOV D,D
	db 00h,03h,49h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 49 CALL ABS
	db 00h,03h,4ah,0e4h,0e2h,0f8h,00h,00h	; 4A MOVW D,D
	db 00h,04h,4bh,0e2h,12h,34h,0f8h,00h	; 4B MOVW D,#
	db 80h,03h,4ch,0e1h,0e0h,0f8h,00h,00h	; 4C MULT D,D
	db 80h,03h,4dh,03h,0e0h,0f8h,00h,00h	; 4D MULT D,#
	db 00h,03h,4eh,00h,0e0h,0f8h,00h,00h	; 4E BMOV BF
	db 00h,03h,4fh,00h,0e0h,0f8h,00h,00h	; 4F BAND BF
; 50-57
	db 00h,03h,50h,12h,0e0h,0f8h,00h,00h	; 50 CMP D,#
	db 00h,03h,51h,12h,0e0h,0f8h,00h,00h	; 51 ADD D,#
	db 00h,03h,52h,12h,0e0h,0f8h,00h,00h	; 52 SUB D,#
	db 00h,03h,53h,12h,0e0h,0f8h,00h,00h	; 53 ADC D,#
	db 00h,03h,54h,12h,0e0h,0f8h,00h,00h	; 54 SBC D,#
	db 00h,03h,55h,12h,0e0h,0f8h,00h,00h	; 55 AND D,#
	db 00h,03h,56h,12h,0e0h,0f8h,00h,00h	; 56 OR D,#
	db 00h,03h,57h,12h,0e0h,0f8h,00h,00h	; 57 XOR D,#
; 58-5F
	db 00h,03h,58h,12h,0e0h,0f8h,00h,00h	; 58 MOV D,#
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 59 INVALID
	db 02h,03h,5ah,04h,12h,0f8h,00h,00h	; 5A CMP @,#
	db 02h,03h,5bh,04h,12h,0f8h,00h,00h	; 5B MOV @,#
	db 00h,00h,00h,00h,00h,00h,00h,00h	; 5C DIV R,R
	db 80h,03h,5dh,03h,0e2h,0f8h,00h,00h	; 5D DIV D,#
	db 00h,04h,5eh,0e0h,0fh,0e1h,0f8h,00h	; 5E MOVM D,D
	db 00h,04h,5fh,0e0h,0fh,12h,0f8h,00h	; 5F MOVM D,#
; 60-67
	db 00h,03h,60h,0e4h,0e2h,0f8h,00h,00h	; 60 CMPW W,W
	db 00h,03h,61h,0e4h,0e2h,0f8h,00h,00h	; 61 ADDW W,W
	db 00h,03h,62h,0e4h,0e2h,0f8h,00h,00h	; 62 SUBW W,W
	db 00h,03h,63h,0e4h,0e2h,0f8h,00h,00h	; 63 ADCW W,W
	db 00h,03h,64h,0e4h,0e2h,0f8h,00h,00h	; 64 SBCW W,W
	db 00h,03h,65h,0e4h,0e2h,0f8h,00h,00h	; 65 ANDW W,W
	db 00h,03h,66h,0e4h,0e2h,0f8h,00h,00h	; 66 ORW W,W
	db 00h,03h,67h,0e4h,0e2h,0f8h,00h,00h	; 67 XORW W,W
; 68-6F
	db 00h,04h,68h,0e2h,12h,34h,0f8h,00h	; 68 CMPW W,#
	db 00h,04h,69h,0e2h,12h,34h,0f8h,00h	; 69 ADDW W,#
	db 00h,04h,6ah,0e2h,12h,34h,0f8h,00h	; 6A SUBW W,#
	db 00h,04h,6bh,0e2h,12h,34h,0f8h,00h	; 6B ADCW W,#
	db 00h,04h,6ch,0e2h,12h,34h,0f8h,00h	; 6C SBCW W,#
	db 00h,04h,6dh,0e2h,12h,34h,0f8h,00h	; 6D ANDW W,#
	db 00h,04h,6eh,0e2h,12h,34h,0f8h,00h	; 6E ORW W,#
	db 00h,04h,6fh,0e2h,12h,34h,0f8h,00h	; 6F XORW W,#
; 70-77
	db 00h,02h,70h,00h,0f8h,00h,00h,00h	; 70 DBNZ R0
	db 00h,02h,71h,00h,0f8h,00h,00h,00h	; 71 DBNZ R1
	db 00h,02h,72h,00h,0f8h,00h,00h,00h	; 72 DBNZ R2
	db 00h,02h,73h,00h,0f8h,00h,00h,00h	; 73 DBNZ R3
	db 00h,02h,74h,00h,0f8h,00h,00h,00h	; 74 DBNZ R4
	db 00h,02h,75h,00h,0f8h,00h,00h,00h	; 75 DBNZ R5
	db 00h,02h,76h,00h,0f8h,00h,00h,00h	; 76 DBNZ R6
	db 00h,02h,77h,00h,0f8h,00h,00h,00h	; 77 DBNZ R7
; 78-7F
	db 00h,03h,78h,12h,34h,0f8h,00h,00h	; 78 MOVW RR0
	db 00h,03h,79h,12h,34h,0f8h,00h,00h	; 79 MOVW RR8
	db 00h,03h,7ah,12h,34h,0f8h,00h,00h	; 7A MOVW RR2
	db 00h,03h,7bh,12h,34h,0f8h,00h,00h	; 7B MOVW RRA
	db 00h,03h,7ch,12h,34h,0f8h,00h,00h	; 7C MOVW RR4
	db 00h,03h,7dh,12h,34h,0f8h,00h,00h	; 7D MOVW RRC
	db 00h,03h,7eh,12h,34h,0f8h,00h,00h	; 7E MOVW RR6
	db 00h,03h,7fh,12h,34h,0f8h,00h,00h	; 7F MOVW RRE
; 80-87
	db 00h,03h,80h,0e0h,00h,0f8h,00h,00h	; 80 BBC B0
	db 00h,03h,81h,0e0h,00h,0f8h,00h,00h	; 81 BBC B1
	db 00h,03h,82h,0e0h,00h,0f8h,00h,00h	; 82 BBC B2
	db 00h,03h,83h,0e0h,00h,0f8h,00h,00h	; 83 BBC B3
	db 00h,03h,84h,0e0h,00h,0f8h,00h,00h	; 84 BBC B4
	db 00h,03h,85h,0e0h,00h,0f8h,00h,00h	; 85 BBC B5
	db 00h,03h,86h,0e0h,00h,0f8h,00h,00h	; 86 BBC B6
	db 00h,03h,87h,0e0h,00h,0f8h,00h,00h	; 87 BBC B7
; 88-8F
	db 00h,03h,88h,0e0h,00h,0f8h,00h,00h	; 88 BBS B0
	db 00h,03h,89h,0e0h,00h,0f8h,00h,00h	; 89 BBS B1
	db 00h,03h,8ah,0e0h,00h,0f8h,00h,00h	; 8A BBS B2
	db 00h,03h,8bh,0e0h,00h,0f8h,00h,00h	; 8B BBS B3
	db 00h,03h,8ch,0e0h,00h,0f8h,00h,00h	; 8C BBS B4
	db 00h,03h,8dh,0e0h,00h,0f8h,00h,00h	; 8D BBS B5
	db 00h,03h,8eh,0e0h,00h,0f8h,00h,00h	; 8E BBS B6
	db 00h,03h,8fh,0e0h,00h,0f8h,00h,00h	; 8F BBS B7
; 90-97
	db 00h,03h,90h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 90 JMP F
	db 00h,03h,91h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 91 JMP LT
	db 00h,03h,92h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 92 JMP LE
	db 00h,03h,93h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 93 JMP ULE
	db 00h,03h,94h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 94 JMP OV
	db 00h,03h,95h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 95 JMP MI
	db 00h,03h,96h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 96 JMP Z
	db 00h,03h,97h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 97 JMP C
; 98-9F
	db 00h,03h,98h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 98 JMP T
	db 00h,03h,99h,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 99 JMP GE
	db 00h,03h,9ah,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9A JMP GT
	db 00h,03h,9bh,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9B JMP UGT
	db 00h,03h,9ch,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9C JMP NOV
	db 00h,03h,9dh,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9D JMP PL
	db 00h,03h,9eh,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9E JMP NZ
	db 00h,03h,9fh,(StaticStubRet>>8),(StaticStubRet&0ffh),0f8h,00h,00h	; 9F JMP NC
; A0-A7
	db 00h,02h,0a0h,0e0h,0f8h,00h,00h,00h	; A0 BCLR B0
	db 00h,02h,0a1h,0e0h,0f8h,00h,00h,00h	; A1 BCLR B1
	db 00h,02h,0a2h,0e0h,0f8h,00h,00h,00h	; A2 BCLR B2
	db 00h,02h,0a3h,0e0h,0f8h,00h,00h,00h	; A3 BCLR B3
	db 00h,02h,0a4h,0e0h,0f8h,00h,00h,00h	; A4 BCLR B4
	db 00h,02h,0a5h,0e0h,0f8h,00h,00h,00h	; A5 BCLR B5
	db 00h,02h,0a6h,0e0h,0f8h,00h,00h,00h	; A6 BCLR B6
	db 00h,02h,0a7h,0e0h,0f8h,00h,00h,00h	; A7 BCLR B7
; A8-AF
	db 00h,02h,0a8h,0e0h,0f8h,00h,00h,00h	; A8 BSET B0
	db 00h,02h,0a9h,0e0h,0f8h,00h,00h,00h	; A9 BSET B1
	db 00h,02h,0aah,0e0h,0f8h,00h,00h,00h	; AA BSET B2
	db 00h,02h,0abh,0e0h,0f8h,00h,00h,00h	; AB BSET B3
	db 00h,02h,0ach,0e0h,0f8h,00h,00h,00h	; AC BSET B4
	db 00h,02h,0adh,0e0h,0f8h,00h,00h,00h	; AD BSET B5
	db 00h,02h,0aeh,0e0h,0f8h,00h,00h,00h	; AE BSET B6
	db 00h,02h,0afh,0e0h,0f8h,00h,00h,00h	; AF BSET B7
; B0-B7
	db 00h,02h,0b0h,0e0h,0f8h,00h,00h,00h	; B0 MOV R0,D
	db 00h,02h,0b1h,0e0h,0f8h,00h,00h,00h	; B1 MOV R1,D
	db 00h,02h,0b2h,0e0h,0f8h,00h,00h,00h	; B2 MOV R2,D
	db 00h,02h,0b3h,0e0h,0f8h,00h,00h,00h	; B3 MOV R3,D
	db 00h,02h,0b4h,0e0h,0f8h,00h,00h,00h	; B4 MOV R4,D
	db 00h,02h,0b5h,0e0h,0f8h,00h,00h,00h	; B5 MOV R5,D
	db 00h,02h,0b6h,0e0h,0f8h,00h,00h,00h	; B6 MOV R6,D
	db 00h,02h,0b7h,0e0h,0f8h,00h,00h,00h	; B7 MOV R7,D
; B8-BF
	db 00h,02h,0b8h,0e0h,0f8h,00h,00h,00h	; B8 MOV D,R0
	db 00h,02h,0b9h,0e0h,0f8h,00h,00h,00h	; B9 MOV D,R1
	db 00h,02h,0bah,0e0h,0f8h,00h,00h,00h	; BA MOV D,R2
	db 00h,02h,0bbh,0e0h,0f8h,00h,00h,00h	; BB MOV D,R3
	db 00h,02h,0bch,0e0h,0f8h,00h,00h,00h	; BC MOV D,R4
	db 00h,02h,0bdh,0e0h,0f8h,00h,00h,00h	; BD MOV D,R5
	db 00h,02h,0beh,0e0h,0f8h,00h,00h,00h	; BE MOV D,R6
	db 00h,02h,0bfh,0e0h,0f8h,00h,00h,00h	; BF MOV D,R7
; C0-C7
	db 00h,02h,0c0h,12h,0f8h,00h,00h,00h	; C0 MOV R0,#
	db 00h,02h,0c1h,12h,0f8h,00h,00h,00h	; C1 MOV R1,#
	db 00h,02h,0c2h,12h,0f8h,00h,00h,00h	; C2 MOV R2,#
	db 00h,02h,0c3h,12h,0f8h,00h,00h,00h	; C3 MOV R3,#
	db 00h,02h,0c4h,12h,0f8h,00h,00h,00h	; C4 MOV R4,#
	db 00h,02h,0c5h,12h,0f8h,00h,00h,00h	; C5 MOV R5,#
	db 00h,02h,0c6h,12h,0f8h,00h,00h,00h	; C6 MOV R6,#
	db 00h,02h,0c7h,12h,0f8h,00h,00h,00h	; C7 MOV R7,#
; C8-CF
	db 00h,02h,0c8h,00h,0f8h,00h,00h,00h	; C8 MOV 10,#
	db 00h,02h,0c9h,00h,0f8h,00h,00h,00h	; C9 MOV 11,#
	db 00h,02h,0cah,00h,0f8h,00h,00h,00h	; CA MOV 12,#
	db 00h,02h,0cbh,00h,0f8h,00h,00h,00h	; CB MOV 13,#
	; P0-P3 are physical board I/O. In particular, P3[7:6] selects the
	; cartridge slot, so no fixed immediate is safe for both slots.
	db 00h,00h,00h,00h,00h,00h,00h,00h	; CC MOV 14,# (board I/O; skip)
	db 00h,00h,00h,00h,00h,00h,00h,00h	; CD MOV 15,# (board I/O; skip)
	db 00h,00h,00h,00h,00h,00h,00h,00h	; CE MOV 16,# (board I/O; skip)
	db 00h,00h,00h,00h,00h,00h,00h,00h	; CF MOV 17,# (cart select; skip)
; D0-D7
	db 00h,02h,0d0h,00h,0f8h,00h,00h,00h	; D0 BR F
	db 00h,02h,0d1h,00h,0f8h,00h,00h,00h	; D1 BR LT
	db 00h,02h,0d2h,00h,0f8h,00h,00h,00h	; D2 BR LE
	db 00h,02h,0d3h,00h,0f8h,00h,00h,00h	; D3 BR ULE
	db 00h,02h,0d4h,00h,0f8h,00h,00h,00h	; D4 BR OV
	db 00h,02h,0d5h,00h,0f8h,00h,00h,00h	; D5 BR MI
	db 00h,02h,0d6h,00h,0f8h,00h,00h,00h	; D6 BR Z
	db 00h,02h,0d7h,00h,0f8h,00h,00h,00h	; D7 BR C
; D8-DF
	db 00h,02h,0d8h,00h,0f8h,00h,00h,00h	; D8 BR T
	db 00h,02h,0d9h,00h,0f8h,00h,00h,00h	; D9 BR GE
	db 00h,02h,0dah,00h,0f8h,00h,00h,00h	; DA BR GT
	db 00h,02h,0dbh,00h,0f8h,00h,00h,00h	; DB BR UGT
	db 00h,02h,0dch,00h,0f8h,00h,00h,00h	; DC BR NOV
	db 00h,02h,0ddh,00h,0f8h,00h,00h,00h	; DD BR PL
	db 00h,02h,0deh,00h,0f8h,00h,00h,00h	; DE BR NZ
	db 00h,02h,0dfh,00h,0f8h,00h,00h,00h	; DF BR NC
; E0-E7
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E0 CALS 10
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E1 CALS 11
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E2 CALS 12
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E3 CALS 13
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E4 CALS 14
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E5 CALS 15
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E6 CALS 16
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E7 CALS 17
; E8-EF
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E8 CALS 18
	db 00h,00h,00h,00h,00h,00h,00h,00h	; E9 CALS 19
	db 00h,00h,00h,00h,00h,00h,00h,00h	; EA CALS 1A
	db 00h,00h,00h,00h,00h,00h,00h,00h	; EB CALS 1B
	db 00h,00h,00h,00h,00h,00h,00h,00h	; EC CALS 1C
	db 00h,00h,00h,00h,00h,00h,00h,00h	; ED CALS 1D
	db 00h,00h,00h,00h,00h,00h,00h,00h	; EE CALS 1E
	db 00h,00h,00h,00h,00h,00h,00h,00h	; EF CALS 1F
; F0-F7
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F0 STOP
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F1 HALT
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F2 ILL F2
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F3 ILL F3
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F4 ILL F4
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F5 ILL F5
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F6 ILL F6
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F7 ILL F7
; F8-FF
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F8 RET
	db 00h,00h,00h,00h,00h,00h,00h,00h	; F9 IRET
	db 00h,01h,0fah,0f8h,00h,00h,00h,00h	; FA CLRC
	db 00h,01h,0fbh,0f8h,00h,00h,00h,00h	; FB COMC
	db 00h,01h,0fch,0f8h,00h,00h,00h,00h	; FC SETC
	db 00h,01h,0fdh,0f8h,00h,00h,00h,00h	; FD EI
	db 00h,01h,0feh,0f8h,00h,00h,00h,00h	; FE DI
	db 00h,01h,0ffh,0f8h,00h,00h,00h,00h	; FF NOP

OpcodeNamePtrs:
	dw OpName00,OpName01,OpName02,OpName03,OpName04,OpName05,OpName06,OpName07
	dw OpName08,OpName09,OpName0A,OpName0B,OpName0C,OpName0D,OpName0E,OpName0F
	dw OpName10,OpName11,OpName12,OpName13,OpName14,OpName15,OpName16,OpName17
	dw OpName18,OpName19,OpName1A,OpName1B,OpName1C,OpName1D,OpName1E,OpName1F
	dw OpName20,OpName21,OpName22,OpName23,OpName24,OpName25,OpName26,OpName27
	dw OpName28,OpName29,OpName2A,OpName2B,OpName2C,OpName2D,OpName2E,OpName2F
	dw OpName30,OpName31,OpName32,OpName33,OpName34,OpName35,OpName36,OpName37
	dw OpName38,OpName39,OpName3A,OpName3B,OpName3C,OpName3D,OpName3E,OpName3F
	dw OpName40,OpName41,OpName42,OpName43,OpName44,OpName45,OpName46,OpName47
	dw OpName48,OpName49,OpName4A,OpName4B,OpName4C,OpName4D,OpName4E,OpName4F
	dw OpName50,OpName51,OpName52,OpName53,OpName54,OpName55,OpName56,OpName57
	dw OpName58,OpName59,OpName5A,OpName5B,OpName5C,OpName5D,OpName5E,OpName5F
	dw OpName60,OpName61,OpName62,OpName63,OpName64,OpName65,OpName66,OpName67
	dw OpName68,OpName69,OpName6A,OpName6B,OpName6C,OpName6D,OpName6E,OpName6F
	dw OpName70,OpName71,OpName72,OpName73,OpName74,OpName75,OpName76,OpName77
	dw OpName78,OpName79,OpName7A,OpName7B,OpName7C,OpName7D,OpName7E,OpName7F
	dw OpName80,OpName81,OpName82,OpName83,OpName84,OpName85,OpName86,OpName87
	dw OpName88,OpName89,OpName8A,OpName8B,OpName8C,OpName8D,OpName8E,OpName8F
	dw OpName90,OpName91,OpName92,OpName93,OpName94,OpName95,OpName96,OpName97
	dw OpName98,OpName99,OpName9A,OpName9B,OpName9C,OpName9D,OpName9E,OpName9F
	dw OpNameA0,OpNameA1,OpNameA2,OpNameA3,OpNameA4,OpNameA5,OpNameA6,OpNameA7
	dw OpNameA8,OpNameA9,OpNameAA,OpNameAB,OpNameAC,OpNameAD,OpNameAE,OpNameAF
	dw OpNameB0,OpNameB1,OpNameB2,OpNameB3,OpNameB4,OpNameB5,OpNameB6,OpNameB7
	dw OpNameB8,OpNameB9,OpNameBA,OpNameBB,OpNameBC,OpNameBD,OpNameBE,OpNameBF
	dw OpNameC0,OpNameC1,OpNameC2,OpNameC3,OpNameC4,OpNameC5,OpNameC6,OpNameC7
	dw OpNameC8,OpNameC9,OpNameCA,OpNameCB,OpNameCC,OpNameCD,OpNameCE,OpNameCF
	dw OpNameD0,OpNameD1,OpNameD2,OpNameD3,OpNameD4,OpNameD5,OpNameD6,OpNameD7
	dw OpNameD8,OpNameD9,OpNameDA,OpNameDB,OpNameDC,OpNameDD,OpNameDE,OpNameDF
	dw OpNameE0,OpNameE1,OpNameE2,OpNameE3,OpNameE4,OpNameE5,OpNameE6,OpNameE7
	dw OpNameE8,OpNameE9,OpNameEA,OpNameEB,OpNameEC,OpNameED,OpNameEE,OpNameEF
	dw OpNameF0,OpNameF1,OpNameF2,OpNameF3,OpNameF4,OpNameF5,OpNameF6,OpNameF7
	dw OpNameF8,OpNameF9,OpNameFA,OpNameFB,OpNameFC,OpNameFD,OpNameFE,OpNameFF

OpName00:	dm 'CLR DIR'
	db 0
OpName01:	dm 'NEG DIR'
	db 0
OpName02:	dm 'COM DIR'
	db 0
OpName03:	dm 'RR DIR'
	db 0
OpName04:	dm 'RL DIR'
	db 0
OpName05:	dm 'RRC DIR'
	db 0
OpName06:	dm 'RLC DIR'
	db 0
OpName07:	dm 'SRL DIR'
	db 0
OpName08:	dm 'INC DIR'
	db 0
OpName09:	dm 'DEC DIR'
	db 0
OpName0A:	dm 'SRA DIR'
	db 0
OpName0B:	dm 'SLL DIR'
	db 0
OpName0C:	dm 'DA DIR'
	db 0
OpName0D:	dm 'SWAP DIR'
	db 0
OpName0E:	dm 'PUSH D'
	db 0
OpName0F:	dm 'POP D'
	db 0
OpName10:	dm 'CMP R,R'
	db 0
OpName11:	dm 'ADD R,R'
	db 0
OpName12:	dm 'SUB R,R'
	db 0
OpName13:	dm 'ADC R,R'
	db 0
OpName14:	dm 'SBC R,R'
	db 0
OpName15:	dm 'AND R,R'
	db 0
OpName16:	dm 'OR R,R'
	db 0
OpName17:	dm 'XOR R,R'
	db 0
OpName18:	dm 'INCW D'
	db 0
OpName19:	dm 'DECW D'
	db 0
OpName1A:	dm 'UNARY @R'
	db 0
OpName1B:	dm 'STK @R'
	db 0
OpName1C:	dm 'BCLR IX'
	db 0
OpName1D:	dm 'BSET IX'
	db 0
OpName1E:	dm 'PUSHW D'
	db 0
OpName1F:	dm 'POPW D'
	db 0
OpName20:	dm 'CMP R,@R'
	db 0
OpName21:	dm 'ADD R,@R'
	db 0
OpName22:	dm 'SUB R,@R'
	db 0
OpName23:	dm 'ADC R,@R'
	db 0
OpName24:	dm 'SBC R,@R'
	db 0
OpName25:	dm 'AND R,@R'
	db 0
OpName26:	dm 'OR R,@R'
	db 0
OpName27:	dm 'XOR R,@R'
	db 0
OpName28:	dm 'MOV R,@R'
	db 0
OpName29:	dm 'MOV @R,R'
	db 0
OpName2A:	dm 'BBC IX'
	db 0
OpName2B:	dm 'BBS IX'
	db 0
OpName2C:	dm 'EXTS D'
	db 0
OpName2D:	dm 'DM #'
	db 0
OpName2E:	dm 'MOV PS0'
	db 0
OpName2F:	dm 'BTST D'
	db 0
OpName30:	dm 'CMP R,@RR'
	db 0
OpName31:	dm 'ADD R,@RR'
	db 0
OpName32:	dm 'SUB R,@RR'
	db 0
OpName33:	dm 'ADC R,@RR'
	db 0
OpName34:	dm 'SBC R,@RR'
	db 0
OpName35:	dm 'AND R,@RR'
	db 0
OpName36:	dm 'OR R,@RR'
	db 0
OpName37:	dm 'XOR R,@RR'
	db 0
OpName38:	dm 'MOV R,@RR'
	db 0
OpName39:	dm 'MOV @RR,R'
	db 0
OpName3A:	dm 'MOVW R,@'
	db 0
OpName3B:	dm 'MOVW @,R'
	db 0
OpName3C:	dm 'MOVW R,R'
	db 0
OpName3D:	dm 'DM D'
	db 0
OpName3E:	dm 'JMP RR'
	db 0
OpName3F:	dm 'CALL RR'
	db 0
OpName40:	dm 'CMP D,D'
	db 0
OpName41:	dm 'ADD D,D'
	db 0
OpName42:	dm 'SUB D,D'
	db 0
OpName43:	dm 'ADC D,D'
	db 0
OpName44:	dm 'SBC D,D'
	db 0
OpName45:	dm 'AND D,D'
	db 0
OpName46:	dm 'OR D,D'
	db 0
OpName47:	dm 'XOR D,D'
	db 0
OpName48:	dm 'MOV D,D'
	db 0
OpName49:	dm 'CALL ABS'
	db 0
OpName4A:	dm 'MOVW D,D'
	db 0
OpName4B:	dm 'MOVW D,#'
	db 0
OpName4C:	dm 'MULT D,D'
	db 0
OpName4D:	dm 'MULT D,#'
	db 0
OpName4E:	dm 'BMOV BF'
	db 0
OpName4F:	dm 'BAND BF'
	db 0
OpName50:	dm 'CMP D,#'
	db 0
OpName51:	dm 'ADD D,#'
	db 0
OpName52:	dm 'SUB D,#'
	db 0
OpName53:	dm 'ADC D,#'
	db 0
OpName54:	dm 'SBC D,#'
	db 0
OpName55:	dm 'AND D,#'
	db 0
OpName56:	dm 'OR D,#'
	db 0
OpName57:	dm 'XOR D,#'
	db 0
OpName58:	dm 'MOV D,#'
	db 0
OpName59:	dm 'INVALID'
	db 0
OpName5A:	dm 'CMP @,#'
	db 0
OpName5B:	dm 'MOV @,#'
	db 0
OpName5C:	dm 'DIV R,R'
	db 0
OpName5D:	dm 'DIV D,#'
	db 0
OpName5E:	dm 'MOVM D,D'
	db 0
OpName5F:	dm 'MOVM D,#'
	db 0
OpName60:	dm 'CMPW W,W'
	db 0
OpName61:	dm 'ADDW W,W'
	db 0
OpName62:	dm 'SUBW W,W'
	db 0
OpName63:	dm 'ADCW W,W'
	db 0
OpName64:	dm 'SBCW W,W'
	db 0
OpName65:	dm 'ANDW W,W'
	db 0
OpName66:	dm 'ORW W,W'
	db 0
OpName67:	dm 'XORW W,W'
	db 0
OpName68:	dm 'CMPW W,#'
	db 0
OpName69:	dm 'ADDW W,#'
	db 0
OpName6A:	dm 'SUBW W,#'
	db 0
OpName6B:	dm 'ADCW W,#'
	db 0
OpName6C:	dm 'SBCW W,#'
	db 0
OpName6D:	dm 'ANDW W,#'
	db 0
OpName6E:	dm 'ORW W,#'
	db 0
OpName6F:	dm 'XORW W,#'
	db 0
OpName70:	dm 'DBNZ R0'
	db 0
OpName71:	dm 'DBNZ R1'
	db 0
OpName72:	dm 'DBNZ R2'
	db 0
OpName73:	dm 'DBNZ R3'
	db 0
OpName74:	dm 'DBNZ R4'
	db 0
OpName75:	dm 'DBNZ R5'
	db 0
OpName76:	dm 'DBNZ R6'
	db 0
OpName77:	dm 'DBNZ R7'
	db 0
OpName78:	dm 'MOVW RR0'
	db 0
OpName79:	dm 'MOVW RR8'
	db 0
OpName7A:	dm 'MOVW RR2'
	db 0
OpName7B:	dm 'MOVW RRA'
	db 0
OpName7C:	dm 'MOVW RR4'
	db 0
OpName7D:	dm 'MOVW RRC'
	db 0
OpName7E:	dm 'MOVW RR6'
	db 0
OpName7F:	dm 'MOVW RRE'
	db 0
OpName80:	dm 'BBC B0'
	db 0
OpName81:	dm 'BBC B1'
	db 0
OpName82:	dm 'BBC B2'
	db 0
OpName83:	dm 'BBC B3'
	db 0
OpName84:	dm 'BBC B4'
	db 0
OpName85:	dm 'BBC B5'
	db 0
OpName86:	dm 'BBC B6'
	db 0
OpName87:	dm 'BBC B7'
	db 0
OpName88:	dm 'BBS B0'
	db 0
OpName89:	dm 'BBS B1'
	db 0
OpName8A:	dm 'BBS B2'
	db 0
OpName8B:	dm 'BBS B3'
	db 0
OpName8C:	dm 'BBS B4'
	db 0
OpName8D:	dm 'BBS B5'
	db 0
OpName8E:	dm 'BBS B6'
	db 0
OpName8F:	dm 'BBS B7'
	db 0
OpName90:	dm 'JMP F'
	db 0
OpName91:	dm 'JMP LT'
	db 0
OpName92:	dm 'JMP LE'
	db 0
OpName93:	dm 'JMP ULE'
	db 0
OpName94:	dm 'JMP OV'
	db 0
OpName95:	dm 'JMP MI'
	db 0
OpName96:	dm 'JMP Z'
	db 0
OpName97:	dm 'JMP C'
	db 0
OpName98:	dm 'JMP T'
	db 0
OpName99:	dm 'JMP GE'
	db 0
OpName9A:	dm 'JMP GT'
	db 0
OpName9B:	dm 'JMP UGT'
	db 0
OpName9C:	dm 'JMP NOV'
	db 0
OpName9D:	dm 'JMP PL'
	db 0
OpName9E:	dm 'JMP NZ'
	db 0
OpName9F:	dm 'JMP NC'
	db 0
OpNameA0:	dm 'BCLR B0'
	db 0
OpNameA1:	dm 'BCLR B1'
	db 0
OpNameA2:	dm 'BCLR B2'
	db 0
OpNameA3:	dm 'BCLR B3'
	db 0
OpNameA4:	dm 'BCLR B4'
	db 0
OpNameA5:	dm 'BCLR B5'
	db 0
OpNameA6:	dm 'BCLR B6'
	db 0
OpNameA7:	dm 'BCLR B7'
	db 0
OpNameA8:	dm 'BSET B0'
	db 0
OpNameA9:	dm 'BSET B1'
	db 0
OpNameAA:	dm 'BSET B2'
	db 0
OpNameAB:	dm 'BSET B3'
	db 0
OpNameAC:	dm 'BSET B4'
	db 0
OpNameAD:	dm 'BSET B5'
	db 0
OpNameAE:	dm 'BSET B6'
	db 0
OpNameAF:	dm 'BSET B7'
	db 0
OpNameB0:	dm 'MOV R0,D'
	db 0
OpNameB1:	dm 'MOV R1,D'
	db 0
OpNameB2:	dm 'MOV R2,D'
	db 0
OpNameB3:	dm 'MOV R3,D'
	db 0
OpNameB4:	dm 'MOV R4,D'
	db 0
OpNameB5:	dm 'MOV R5,D'
	db 0
OpNameB6:	dm 'MOV R6,D'
	db 0
OpNameB7:	dm 'MOV R7,D'
	db 0
OpNameB8:	dm 'MOV D,R0'
	db 0
OpNameB9:	dm 'MOV D,R1'
	db 0
OpNameBA:	dm 'MOV D,R2'
	db 0
OpNameBB:	dm 'MOV D,R3'
	db 0
OpNameBC:	dm 'MOV D,R4'
	db 0
OpNameBD:	dm 'MOV D,R5'
	db 0
OpNameBE:	dm 'MOV D,R6'
	db 0
OpNameBF:	dm 'MOV D,R7'
	db 0
OpNameC0:	dm 'MOV R0,#'
	db 0
OpNameC1:	dm 'MOV R1,#'
	db 0
OpNameC2:	dm 'MOV R2,#'
	db 0
OpNameC3:	dm 'MOV R3,#'
	db 0
OpNameC4:	dm 'MOV R4,#'
	db 0
OpNameC5:	dm 'MOV R5,#'
	db 0
OpNameC6:	dm 'MOV R6,#'
	db 0
OpNameC7:	dm 'MOV R7,#'
	db 0
OpNameC8:	dm 'MOV 10,#'
	db 0
OpNameC9:	dm 'MOV 11,#'
	db 0
OpNameCA:	dm 'MOV 12,#'
	db 0
OpNameCB:	dm 'MOV 13,#'
	db 0
OpNameCC:	dm 'MOV 14,#'
	db 0
OpNameCD:	dm 'MOV 15,#'
	db 0
OpNameCE:	dm 'MOV 16,#'
	db 0
OpNameCF:	dm 'MOV 17,#'
	db 0
OpNameD0:	dm 'BR F'
	db 0
OpNameD1:	dm 'BR LT'
	db 0
OpNameD2:	dm 'BR LE'
	db 0
OpNameD3:	dm 'BR ULE'
	db 0
OpNameD4:	dm 'BR OV'
	db 0
OpNameD5:	dm 'BR MI'
	db 0
OpNameD6:	dm 'BR Z'
	db 0
OpNameD7:	dm 'BR C'
	db 0
OpNameD8:	dm 'BR T'
	db 0
OpNameD9:	dm 'BR GE'
	db 0
OpNameDA:	dm 'BR GT'
	db 0
OpNameDB:	dm 'BR UGT'
	db 0
OpNameDC:	dm 'BR NOV'
	db 0
OpNameDD:	dm 'BR PL'
	db 0
OpNameDE:	dm 'BR NZ'
	db 0
OpNameDF:	dm 'BR NC'
	db 0
OpNameE0:	dm 'CALS 10'
	db 0
OpNameE1:	dm 'CALS 11'
	db 0
OpNameE2:	dm 'CALS 12'
	db 0
OpNameE3:	dm 'CALS 13'
	db 0
OpNameE4:	dm 'CALS 14'
	db 0
OpNameE5:	dm 'CALS 15'
	db 0
OpNameE6:	dm 'CALS 16'
	db 0
OpNameE7:	dm 'CALS 17'
	db 0
OpNameE8:	dm 'CALS 18'
	db 0
OpNameE9:	dm 'CALS 19'
	db 0
OpNameEA:	dm 'CALS 1A'
	db 0
OpNameEB:	dm 'CALS 1B'
	db 0
OpNameEC:	dm 'CALS 1C'
	db 0
OpNameED:	dm 'CALS 1D'
	db 0
OpNameEE:	dm 'CALS 1E'
	db 0
OpNameEF:	dm 'CALS 1F'
	db 0
OpNameF0:	dm 'STOP'
	db 0
OpNameF1:	dm 'HALT'
	db 0
OpNameF2:	dm 'ILL F2'
	db 0
OpNameF3:	dm 'ILL F3'
	db 0
OpNameF4:	dm 'ILL F4'
	db 0
OpNameF5:	dm 'ILL F5'
	db 0
OpNameF6:	dm 'ILL F6'
	db 0
OpNameF7:	dm 'ILL F7'
	db 0
OpNameF8:	dm 'RET'
	db 0
OpNameF9:	dm 'IRET'
	db 0
OpNameFA:	dm 'CLRC'
	db 0
OpNameFB:	dm 'COMC'
	db 0
OpNameFC:	dm 'SETC'
	db 0
OpNameFD:	dm 'EI'
	db 0
OpNameFE:	dm 'DI'
	db 0
OpNameFF:	dm 'NOP'
	db 0
