	AREA storage,DATA
array
	DCD 3, 7, 31, 193, 2, -5, -232
array_end

	AREA main,CODE
	EXPORT asm_main
asm_main
<<<<<<< HEAD
	LDR r0, =array
	LDR r1, =array_end
	MOV r2, #0
sum_loop
	CMP r0, r1
	BHS sum_loop_end
	
	LDR r3, [r0], #4
	BL mod
	ADD r2, r3
	B sum_loop
sum_loop_end

	B .			; Loop forever
	
mod
	CMP r3, #0
	RSBLE r3, r3, #0
	BX lr
end_mod
=======
	; Your code goes here

 
	B .			; Loop forever
>>>>>>> 636612f05527fd6df80fef13f2ebb96646dc3435

	ALIGN
	END
