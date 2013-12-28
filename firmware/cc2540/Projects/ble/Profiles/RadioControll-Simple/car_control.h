#ifndef _CAR_CONTROL_H_
#define _CAR_CONTROL_H_

#include "hal_types.h"

//#define CAR_TYPE_RC		0
//#define CAR_TYPE_LAME	1

#ifdef __cplusplus
extern "C"
{
#endif

extern void CarInit();
extern void CarSetSteering(uint16 value);
extern void CarSetThrottle(uint16 value);
extern void CarConnected(uint8 taskId);
extern void CarDisConnected();
extern void CarUpdateStatus();
extern void CarTick();
extern void CarSetLights(uint16 value);

#ifdef __cplusplus
}
#endif

#endif
