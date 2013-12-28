#include "car_control.h"
#include "car_control_lame.h"

void CarInit()
{
	CarLameInit();
};

void CarSetSteering(uint16 value)
{
	CarLameSetSteering(value);
};

void CarSetThrottle(uint16 value)
{
	CarLameSetThrottle(value);
};

void CarConnected(uint8 taskId)
{
	CarLameConnected();
};

void CarDisConnected()
{
	CarLameDisConnected();
};

void CarUpdateStatus()
{
	CarLameUpdateStatus();
};

void CarTick()
{
};

void CarSetLights(uint16 value)
{
};
