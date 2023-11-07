#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Utils/utils.h"

int main(void) {
	configClock();
	configUSART2(38400);

	uint32_t x=3, y=5, z=0;
  __asm volatile (
//		"MOV %[input2], %[output]\n\t"
    "ADD %[input2], %[input], %[input], LSL #3\n\t" /* z = x*9 */
    "ADD %[input2], %[input2], %[input], LSL #2\n\t" /* z = z +(x*4) */
		"ADD %[output], %[output], %[input2]" /*y = y + z = y + z +(x*4)*/
    : [output] "+&r" (y)
    : [input] "r" (x), [input2] "r" (z)
	);
	
	printf("y = %" PRIu32 "\n", y);

	while(1);
}
