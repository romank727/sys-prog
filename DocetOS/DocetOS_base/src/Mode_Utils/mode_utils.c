#include "Mode_Utils/mode_utils.h"
#include <stdio.h>
#include <inttypes.h>

#define PSR_HANDLER_MASK 0x1FF
#define nPRIV_MASK 0x1
#define SPSEL_MASK 0x2

void reportState() {
	
	uint32_t psr = getPSR();
	uint32_t psrLastBits = psr & PSR_HANDLER_MASK;

	uint32_t control = getCONTROL();
	uint32_t npriv = control & nPRIV_MASK;
	uint32_t spsel = control & SPSEL_MASK;
	
}
