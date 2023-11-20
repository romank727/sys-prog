#include "Mode_Utils/mode_utils.h"
#include <stdio.h>

#define PSR_HANDLER_MASK 0x1FF
#define nPRIV_MASK 0x1
#define SPSEL_MASK 0x2

void reportState(void) {

	uint32_t psr = getPSR();
	uint32_t psrLastBits = psr & PSR_HANDLER_MASK;

	uint32_t control = getCONTROL();
	uint32_t npriv = control & nPRIV_MASK;
	uint32_t spsel = control & SPSEL_MASK;
	
	// if psrLastBits = 0, then ThreadMode, otherwise Handler mode
	const char* mode = (psrLastBits == 0) ? "Thread mode" : "Handler mode";
	const char* privilege = (npriv == 0) ? "privileged" : "un-privileged";
	const char* stackPointer = (spsel ==0) ? "MSP" : "PSP";
	
	printf("\r\n%s, %s, %s\r\n", mode, privilege, stackPointer);
}
