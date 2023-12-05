	AREA mainarea,CODE
	EXPORT asm_main

asm_main
	MOV	r0, #0
	MOV r1, #0
loop
	ADD	r0, #1
	ADD	r1, r0
	; compare r0 to 10
	CMP	r0, #10
	; if r0 is less than 10, branch back to loop
	BLT loop
	; r0 goes from 0 to 9 inclusive
	; r1 sums up to 45
	
	B .			; Loop forever

	END
