	AREA mainarea,CODE
	EXPORT process

process
	STMFD sp!, {r4-r8, lr}
	
	LDMIA r0, {r4-r7}
	ADD r4, r4, r5
	STR r4, [r0]
	
	MOV r0, r7
	BLX r6
	
	LDMFD sp!, {r4-r8, pc}
	
	END
