#include "Mode_Utils/mode_utils.h"
#include <stdio.h>
#include <inttypes.h>

#define PSR_HANDLER_MASK 0x1FF
#define nPRIV_MASK 0x1
#define SPSEL_MASK 0x2

uint32_t argument_delegate(_OS_SVC_StackFrame_t * const stack);

void reportState(void) {

	uint32_t psr = getPSR();
	uint32_t psrLastBits = psr & PSR_HANDLER_MASK;
	
	uint32_t control = getCONTROL();
	uint32_t npriv = control & nPRIV_MASK;
	uint32_t spsel = control & SPSEL_MASK;
	
	// if psrLastBits = 0, then ThreadMode, otherwise Handler mode
	char* mode = (!psrLastBits) ? "Thread mode" : "Handler mode";
	char* privilege = (!npriv || psrLastBits) ? "privileged" : "un-privileged";
	char* stackPointer = (!spsel) ? "MSP" : "PSP";
	
	printf("\r\n%s, %s, %s\r\n", mode, privilege, stackPointer);
}

uint32_t argument_delegate(_OS_SVC_StackFrame_t * const stack) {
	reportState();
	printf("\r\ndelegate stack r0: %" PRIu32 "\n\r", stack->r0);
	stack->r0++;
	printf("\r\ndelegate stack r0: %" PRIu32 "\n\r", stack->r0);
	
	return stack->r0;
}
