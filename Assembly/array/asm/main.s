	AREA storage,DATA
array
	; Total is 232 for the example list
	DCD 3, 7, 31, 193, 2, -5, -232
array_end

	AREA main,CODE
	EXPORT asm_main
asm_main
	; Your code goes here
	; MOV r0, -5
	
	LDR r0, =array
	LDR r1, =array_end
	MOV r2, #0
	
sum_loop
	CMP r0, r1
	BHS sum_loop_end
	
	LDR r3, [r0], #4
	ADD r2, r3
	B sum_loop

sum_loop_end
	BL abs
	B .			; Loop forever
abs
	CMP r0, #0
	RSBLE r0, r0, #0
	BX lr
	
	ALIGN
	END
