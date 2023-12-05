	AREA mainarea,CODE
	EXPORT calculate

calculate
	STMFD sp!, {r4, lr}
	MOV r2, r0, LSL #2
	ADD r2, r2, r0
	ADD r0, r2, r1
	LDMFD sp!, {r4, pc}
	
	
	END
