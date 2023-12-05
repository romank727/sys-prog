	AREA mainarea,CODE
	IMPORT fib
	EXPORT asm_main

asm_main

	MOV r0, #7  ; Input Fibonacci index
	BL fib      ; Call fib function
	B .         ; Loop forever

	ALIGN
	END