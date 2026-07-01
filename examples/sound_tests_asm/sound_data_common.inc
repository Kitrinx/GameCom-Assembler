; Shared sound waveform/sample data.

Sg0Wave:
	db 01h,23h,45h,67h,76h,54h,32h,10h
	db 0fh,0edh,0cbh,98h,89h,0abh,0cdh,0f0h

Sg1Wave:
	db 77h,66h,55h,44h,33h,22h,11h,00h
	db 88h,99h,0aah,0bbh,0cch,0ddh,0eeh,0ffh

DacSamples:
	db 80h,98h,0aeh,0c1h,0d0h,0dah,0dfh,0dah
	db 0d0h,0c1h,0aeh,98h,80h,68h,52h,3fh
	db 30h,26h,21h,26h,30h,3fh,52h,68h
	db 80h,0a4h,0c4h,0dah,0e0h,0dah,0c4h,0a4h
	db 80h,5ch,3ch,26h,20h,26h,3ch,5ch
	db 80h,90h,80h,70h,80h,0a0h,80h,60h
	db 80h,0b0h,80h,50h,80h,0c0h,80h,40h
	db 80h,0a8h,88h,58h,78h,98h,70h,80h

OverflowPosWave:
	db 77h,77h,77h,77h,77h,77h,77h,77h
	db 77h,77h,77h,77h,77h,77h,77h,77h
	db 77h,77h,77h,77h,77h,77h,77h,77h
	db 77h,77h,77h,77h,77h,77h,77h,77h

OverflowNegWave:
	db 88h,88h,88h,88h,88h,88h,88h,88h
	db 88h,88h,88h,88h,88h,88h,88h,88h
	db 88h,88h,88h,88h,88h,88h,88h,88h
	db 88h,88h,88h,88h,88h,88h,88h,88h
