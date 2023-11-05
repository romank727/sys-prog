	AREA mainarea,CODE
	EXPORT fib

fib
    PUSH    {R1, LR}  ; Save R1 and LR

    ; Check for base cases
    CMP     R0, #1
    BEQ     base_case
    CMP     R0, #2
    BEQ     base_case

    ; Recursive case: calculate fib(n-1)
    PUSH    {R0}      ; Save R0
    SUBS    R0, R0, #1
    BL      fib
    MOV     R1, R0    ; Save the result in R1
    
    ; Recursive case: calculate fib(n-2)
    POP     {R0}      ; Restore R0
    SUBS    R0, R0, #2
    BL      fib

    ; Sum fib(n-1) and fib(n-2) to get fib(n)
    ADD     R0, R0, R1

    ; Pop saved registers and return
    POP     {R1, LR}
    BX      LR

base_case
    ; Base case: return 1
    MOV     R0, #1
    POP     {R1, LR}
    BX      LR
	
	END
