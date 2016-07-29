#ifndef _CAR_CONTROL_H_
#define _CAR_CONTROL_H_

#include "hal_types.h"

#define START_DEVICE_EVENT         0x0001
#define PERIODIC_EVENT             0x0002
#define UPDATE_STATUS_EVENT        0x0004
#define ACCELEROMETER_SENSOR_EVENT 0x0008
#define NO_COMMAND_EVENT           0x0010
#define PPM_EVENT                  0x0020
#define CONNECTION_UPDATE_EVENT    0x0040
#define TEMPERATURE_READING_EVENT  0x0200

#ifdef __cplusplus
extern "C"
{
#endif

extern uint8* defaultName;
extern uint8 deviceType;
extern bool keepRunninOnNoCommands;

extern void CarInit(uint8 taskId);
extern void CarSetSteering(uint16 value);
extern void CarSetThrottle(uint16 value);
extern void CarConnected();
extern void CarDisConnected();
extern void CarUpdateStatus();
extern void CarTick();
extern void CarSetDefaultValues();
extern void CarConnectBattery();
extern void CarDisConnectBattery();
extern void CarInitBattery();
extern void CarInitLights();
extern void CarSetLights(uint16 value);
extern void CarSetLight(uint16 lightId, uint8 state);
extern void CarPPMTick();
extern void FinishAccelerometerUpdate();
extern void TempFinishUpdate();

#define DECLARE_BATTERY_CONNECT(selectPinCode, selectOutputCode, pin, onState) \
\
void CarConnectBattery() \
{ \
	pin = onState; \
};\
\
void CarDisConnectBattery()  \
{ \
	pin = !onState; \
}; \
\
void CarInitBattery() \
{ \
	selectPinCode; \
	selectOutputCode; \
};

#define DECLARE_LIGHTS(selectPinCode, selectOutputCode, LIGHTS0_PIN, LIGHTS1_PIN, LIGHTS2_PIN, LIGHTS3_PIN, LIGHTS4_PIN, INVERTED) \
\
void CarInitLights() \
{ \
	selectPinCode; \
	selectOutputCode; \
}; \
\
void CarSetLight(uint16 lightId, uint8 state) \
{ \
	if (INVERTED) \
	{ \
		state = !state; \
	} \
\
	switch (lightId) \
	{ \
		case 0: \
			LIGHTS0_PIN = state; \
			break; \
		case 1: \
			LIGHTS1_PIN = state; \
			break; \
		case 2: \
			LIGHTS2_PIN = state; \
			break; \
		case 3: \
			LIGHTS3_PIN = state; \
			break; \
		case 4: \
			LIGHTS4_PIN = state; \
			break; \
		default: \
			break; \
	} \
};

#ifdef __cplusplus
}
#endif

#endif
