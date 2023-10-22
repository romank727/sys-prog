	AREA mainarea,CODE
	EXPORT asm_main
asm_main

	; Your instructions go here...
	
	; multiply by 8 (LSL by 3)
	; multiply by 4 (LSL by 2)
	; add 1 instance of original number -> = 13x
	
	MOV r1, r0		; copy original number into r1
	LSL r0, r0, #3	; multiply by 8 (shift left by 3)
	LSL r1, r1, #2	; multiply by 4 (shift left by 2)
	ADD r0, r0, r1	; add them -> 12x
	
	ADD r0, r0, r1, LSR #2	; add the 12x with the original number -> 13x
	
	B .			; Loop forever

	END
