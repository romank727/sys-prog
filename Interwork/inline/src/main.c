#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

int main(void) {
	configClock();
	configUSART2(38400);

	uint32_t x=3, y=5, z=0;
	__asm volatile (
		"ADD r0, %[in1], %[in2], LSL #3\n\t" /* x + y*8 */
		"ADD %[output], r0, %[in1], LSL #2" /* +(x*4) = x*5 + y*8 */
		: [output] "=&r" (z)
		: [in1] "r" (x), [in2] "r" (y)
		: "r0"
	);

	printf("z = %" PRIu32 "\n", z);

	while(1);
}
