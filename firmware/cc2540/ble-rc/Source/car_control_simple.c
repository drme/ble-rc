#if defined(FIRMWARE_TYPE_SIMPLE)

#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "hci.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "blercprofile.h"
#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "blercprofile.h"
#include "hal_adc.h"

DECLARE_BATTERY_CONNECT(P1DIR |= 0x40, P1SEL &= ~0x40, P1_6, 0); // not conencted anyways.
DECLARE_LIGHTS(P0DIR |= 0x87, P0SEL &= ~0x87, P0_0, P0_1, P0_2, P0_7, P1_0, FALSE);  // P1_0 not connected

uint8* defaultName = "Radio Controlled Toy\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint8 deviceType = 0x03;
bool keepRunninOnNoCommands = true;
static float maxValue = 700.0f;

void CarSetDefaultValues()
{
	settings.throttleMin    = 0;
	settings.throttleMax    = 700;
	settings.throttleCenter = 350;
	settings.steeringMin    = 0;
	settings.steeringMax    = 700;
	settings.steeringCenter = 350;
	settings.deviceCapabilities	= HasLights;
};

static void CarReset()
{
	T1CC4L = 0;
	T1CC4H = 0;	

	T1CC3L = 0;
	T1CC3H = 0;

	T1CC1L = 0;
	T1CC1H = 0;	

	T1CC2L = 0;
	T1CC2H = 0;

	T1CNTL = 0;		
};

void CarInit(uint8 taskId)
{
	uint8 pwmPins = (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6); // P0.3, P0.4, P0.5, and P0.6 as PWM

	P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
	P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

	PERCFG |= 0x03;     //  Move USART1&2 to alternate2 location so that T1 is visible
  /*
//	T1CC0L = 0xff;
//	T1CC0H = 0xff;

	uint8 mode = 0x24;
	
//	T1CCTL0 = mode;//(T1CCTL0 & ~0x23) | 0x1C; // Channel 0 in compare mode, Set output on compare-up, clear on 0 (50% duty cycle)	
	T1CCTL1 = mode;  //(T1CCTL1 & ~0x23) | 0x1C; // Channel 1 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL2 = mode; //(T1CCTL2 & ~0x23) | 0x1C;  // Channel 2 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL3 = mode; //(T1CCTL3 & ~0x23) | 0x1C; // Channel 3 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL4 = mode; //(T1CCTL4 & ~0x23) | 0x1C;  // Channel 4 in compare mode, Set output on compare-up, clear on compare-down
						
	T1CTL = 0x0d; //0x06;// 0x0A; // divide with 128 and to do i up-down mode
*/
	
	T1CC0L = 0xFF;
	T1CC0H = 0xff;

	T1CCTL0 = (T1CCTL0 & ~0x23) | 0x1C; // Channel 0 in compare mode, Set output on compare-up, clear on 0 (50% duty cycle)	
	T1CCTL1 = (T1CCTL1 & ~0x23) | 0x1C; // Channel 1 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL2 = (T1CCTL2 & ~0x23) | 0x1C;  // Channel 2 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL3 = (T1CCTL3 & ~0x23) | 0x1C; // Channel 3 in compare mode, Set output on compare-up, clear on compare-down
	T1CCTL4 = (T1CCTL4 & ~0x23) | 0x1C;  // Channel 4 in compare mode, Set output on compare-up, clear on compare-down
						
	T1CTL |= 0x06;// 0x0A; // divide with 128 and to do i up-down mode
	
	CarReset();
};

void CarSetSteering(uint16 value)
{
	if (value > 400)
	{
		if (value > 695)
		{
			value = 695;
		}
	
		float maxDiff = maxValue - 400.0f;
		float diff = maxValue - value;
	
		float vv = ((float)0xfffe * diff / maxDiff);
		
		uint16 v = (uint16)vv;
		
		T1CC1L = 0;
		T1CC1H = 0;  		
		
		T1CC2L = (uint8)v;
		T1CC2H = (uint8)(v >> 8);	
	}
	else if (value < 300)
	{
		if (value < 3)
		{
			value = 3;
		}	
	
		float maxDiff = 300.0f;
		float diff = value;

		float vv = ((float)0xfffe * diff / maxDiff);
		
		uint16 v = (uint16)vv;

		T1CC2L = 0;
		T1CC2H = 0;	

		T1CC1L = (uint8)v;
	    T1CC1H = (uint8)(v >> 8);
	}
	else
	{
		T1CC1L = 0;
		T1CC1H = 0;	
	
		T1CC2L = 0;
		T1CC2H = 0;
	}
	
	T1CNTL = 0;		
};

void CarSetThrottle(uint16 value)
{
	if (value > 400)
	{
		if (value > 695)
		{
			value = 695;
		}
	
		float maxDiff = maxValue - 400.0f;
		float diff = maxValue - value;
	
		float vv = ((float)0xfffe * diff / maxDiff);
		
		uint16 v = (uint16)vv;
		
		T1CC3L = 0;
		T1CC3H = 0;  		
		
		T1CC4L = (uint8)v;
		T1CC4H = (uint8)(v >> 8);	
	}
	else if (value < 300)
	{
		if (value < 3)
		{
			value = 3;
		}	
	
		float maxDiff = 300.0f;
		float diff = value;

		float vv = ((float)0xfffe * diff / maxDiff);
		
		uint16 v = (uint16)vv;

		T1CC4L = 0;
		T1CC4H = 0;	

		T1CC3L = (uint8)v;
		T1CC3H = (uint8)(v >> 8);
	}
	else
	{
		T1CC3L = 0;
		T1CC3H = 0;	
	
		T1CC4L = 0;
		T1CC4H = 0;
	}
	
	T1CNTL = 0;		
};

void CarConnected()
{
	CarReset();
};

void CarDisConnected()
{
	CarReset();
};

void CarUpdateStatus()
{
};

void CarTick()
{
};

void CarPPMTick()
{
};

void FinishAccelerometerUpdate()
{
};

void TempFinishUpdate()
{
};

#endif
