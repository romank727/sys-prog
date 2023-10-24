	AREA storage,DATA
array
	SPACE 4*20	; Declares a 20-word storage area
array_end

	AREA mainarea,CODE
	EXPORT asm_main

; check if required amount of fib numbers needed is even or odd using bit mask or sumn (change 20 to some other number)
; if required is even -> proceed with loop that finds 2 fib numbers and stores them
; if required is odd -> calc the first fib number outside the loop and then branch to the usual loop


asm_main
	LDR r0, =array;
	LDR r1, =array_end
	
	; required num of fib numbers
    SUB r2, r1, r0  ; r2 = array_end - array
    LSR r2, r2, #2  ; each fib number is 4 bytes, so r2/4
	
	; even or odd
    AND r3, r2, #1  ; r3 = r2 % 2
	
	; store the first '1'
    MOV r4, #1
    STR r4, [r0], #4
	
    ; if total is odd, store the second '1'
    CMP r3, #1
	MOVEQ r5, #1
    STREQ r5, [r0], #4
	
	
	
loop
	
	
	
	
end_loop
	
	
	
	
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
