#ifndef MODE_UTILS_H
#define MODE_UTILS_H

#include <stdint.h>

#define OS_INTERNAL
#include "OS/os.h"

#define reportState_SVC(x) _svc_1(x, SVC_WITH_ARGS)

uint32_t getPSR(void);
uint32_t getCONTROL(void);
void reportState(void);

#endif /* MODE_UTILS_H */
