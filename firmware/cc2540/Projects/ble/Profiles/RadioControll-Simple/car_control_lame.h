#ifndef _CAR_CONTROL_LAME_H_
#define _CAR_CONTROL_LAME_H_

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void CarLameInit();
extern void CarLameSetSteering(uint16 value);
extern void CarLameSetThrottle(uint16 value);
extern void CarLameConnected();
extern void CarLameDisConnected();
extern void CarLameUpdateStatus();

#ifdef __cplusplus
}
#endif

#endif
