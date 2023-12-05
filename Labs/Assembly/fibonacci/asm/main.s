	AREA storage,DATA
array
	SPACE 4*25	; Declares a 20-word storage area
array_end

	AREA mainarea,CODE
	EXPORT asm_main

asm_main
	LDR r0, =array;
	LDR r1, =array_end
	
	MOV r2, #1
	MOV r3, #1
	
	BL loop
	
	B .			; Loop forever
	
loop
	STR r2, [r0], #4
	CMP r0, r1
	BXHS lr				; branch to link reg
	
	STR r3, [r0], #4
	CMP r0, r1
	BXHS lr
	
	ADD r2, r2, r3
	ADD r3, r3, r2
	
	B loop
end_loop
	
	
	; Single digit fibonacci
	
;	LDR r0, =array
;	LDR r1, =array_end
;	
;	MOV r2, #1
;	MOV r3, #1

;loop
;	CMP r0, r1
;	BHS end_loop		; branch to end if r0 >= r1

;	STR r2, [r0], #4	; 1, 		1, 2, 	1, 2, 3,	1, 2, 3, 5,		1, 2, 3, 5, 8,		1, 2, 3, 5, 8, 13,   	.....
;	MOV r4, r2			; r4 = 1	r4 = 2	r4 = 3		r4 = 5			r4 = 8				r4 = 13
;	ADD r2, r3			; r2 = 2	r2 = 3	r2 = 5		r2 = 8			r2 = 13				r2 = 21
;	MOV r3, r4			; r3 = 1	r3 = 2	r3 = 3		r3 = 5			r3 = 8				r3 = 13
;	
;	B loop
;end_loop

	B .			; Loop forever

	ALIGN
	END
