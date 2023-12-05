	AREA mainarea,CODE
	IMPORT report
	EXPORT fib
		
fib
	STMFD sp!, {r4-r8, lr}
	MOV r4, #1			; first 2 fib numbers
	MOV r5, #1
	MOV r8, r0			; counter, r8 = 20
	
loop
	CMP r8, #0
	BLS loop_end
	SUB r8, r8, #1
	
	MOV r0, r4
	BL report
	
	ADD r6, r4, r5		; Find new fib number
	MOV r4, r5			; Update the previous 2 fib numbers
	MOV r5, r6
	
	B loop
loop_end

	LDMFD sp!, {r4-r8, pc}

	END
