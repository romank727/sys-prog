	AREA mainarea,CODE
	EXPORT process

process
	STMFD sp!, {r4-r8, lr}
	
	LDMIA r0, {r4-r7}
	ADD r5, r4, r5
	STR r5, [r0, #4]
	
	MOV r0, r6
	BLX r7
	
	LDMFD sp!, {r4-r8, pc}
	
	END
