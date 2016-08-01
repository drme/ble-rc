#if defined(FIRMWARE_TYPE_TANK)

#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "blercprofile.h"
#include "hal_adc.h"

//#define M1_1	P0_3
//#define M1_2	P0_4
//#define M2_1	P0_5
//#define M2_2	P0_6
//#define H1		M1_2 = M2_1
//#define H2		M1_1 = M2_2

static uint16 lastThrottle = 350;
static uint8 mainTaskId = 0;
static bool connected = false;
static float maxValue = 700.0f;


void CarSetDefaultValues()
{
	blePulseWidth			= 20;
	bleThrottleMin			= 160;
	bleThrottleMax			= 590;
	bleThrottleCenter		= 375;
	bleSteeringMin			= 250;
	bleSteeringMax			= 500;
	bleSteeringCenter		= 375;
	bleBatteryMaxVoltage	= 72;
	bleBatteryCapacity		= 5000;
};

void CarReset()
{
//	H1 = 0x0;
//	H2 = 0x0;
  
  
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

void CarRCInitPWMTimers()
{
	uint8 pwmPins = 0x78; // P0.5, P0.6, P0.4, and P0.3 as PWM

	P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
	P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

	PERCFG |= 0x03;     //  Move USART1&2 to alternate2 location so that T1 is visible
  
	T1CC0L = 0xFF;
	T1CC0H = 0xff;

	T1CCTL0 = (T1CCTL0 & ~0x23) | 0x1C; // Channel 0 in compare mode, Set output on compare-up, clear on 0 (50% duty cycle)	

	T1CC1L = 0x1 * 0;  // PWM duty cycle, Channel 1 (P0_3)
	T1CC1H = 0x0;
	T1CCTL1 = (T1CCTL1 & ~0x23) | 0x1C; // Channel 1 in compare mode, Set output on compare-up, clear on compare-down
   
	T1CC2L = 0x1 * 0;  // PWM duty cycle, Channel 2 (P0_4)
	T1CC2H = 0x00;
	T1CCTL2 = (T1CCTL2 & ~0x23) | 0x1C;  // Channel 2 in compare mode, Set output on compare-up, clear on compare-down

	T1CC3L = 0x1 * 0;  // PWM duty cycle, Channel 3 (P0_4)
	T1CC3H = 0x00;
	T1CCTL3 = (T1CCTL3 & ~0x23) | 0x1C; // Channel 3 in compare mode, Set output on compare-up, clear on compare-down

	T1CC4L = 0x1 * 0;  // PWM duty cycle, Channel 4 (P0_5)
	T1CC4H = 0x00;
	T1CCTL4 = (T1CCTL4 & ~0x23) | 0x1C;  // Channel 4 in compare mode, Set output on compare-up, clear on compare-down
						
	T1CTL |= 0x06;// 0x0A; // divide with 128 and to do i up-down mode

	T1CNTL = 0;			// Reset timer to 0;

	CarReset();
};


void CarInit(uint8 taskId)
{
//	P0DIR = P1DIR = P2DIR = 0xff;
//	P0SEL = P1SEL = P2SEL = 0x00;
//	P0 = P1 = P2 = 0x00;

//	CarLameReset();
  
  
  	mainTaskId = taskId;
	
	P0SEL	= 0x00;
	P0DIR	= 0x00;
	P0		= 0x00;
  
 	P1SEL	= 0x00;
	P1DIR	= 0x00;
	P1		= 0x00;

	P2SEL	= 0x00;
	P2DIR	= 0x00;
	P2		= 0x00;
	
	CarRCInitPWMTimers();

	//P1DIR |= (1 << 6);
	
//	P1_6 = 1;
	
	
	//IEN0 |= 0x02; // ADC interupt

};

void CarSetThrottle(uint16 value)
{
	lastThrottle = value;
  
/*	if (value < 350)
	{
		H1 = 1;
		H2 = 0;
	}
	else if (value > 400)
	{
		H1 = 0;
		H2 = 1;
	}
	else
	{
		H1 = 0;
		H2 = 0;
	} */
  
	if (value > 450)
	{
		float maxDiff = maxValue - 450.0f;
		float diff = maxValue - value;
	  
		uint16 v = (uint16)((float)0xfffe * diff / maxDiff);
		
		T1CC4L = T1CC1L = (uint8)v;
		T1CC4H = T1CC1H = (uint8)(v >> 8);	

		T1CC3L = T1CC2L = 0;
		T1CC3H = T1CC2H = 0;  
	}
	else if (value < 300)
	{
		float maxDiff = 300.0f;
		float diff = value;

		uint16 v = (uint16)((float)0xfffe * diff / maxDiff);

		T1CC4L = T1CC1L = 0;
		T1CC4H = T1CC1H = 0;	

		T1CC3L = T1CC2L = (uint8)v;
		T1CC3H = T1CC2H = (uint8)(v >> 8);  
	}
	else
	{
		T1CC4L = T1CC1L = 0;
		T1CC4H = T1CC1H = 0;	
	
		T1CC3L = T1CC2L = 0;
		T1CC3H = T1CC2H = 0;  
	}  
	
	T1CNTL = 0;		
  
  
};

void CarSetSteering(uint16 value)
{
	if (value > 450)
	{
		float maxDiff = maxValue - 450.0f;
		float diff = maxValue - value;
	  
		uint16 v = (uint16)((float)0xfffe * diff / maxDiff);
		
		T1CC3L = T1CC1L = (uint8)v;
		T1CC3H = T1CC1H = (uint8)(v >> 8);	

		T1CC4L = T1CC2L = 0;
		T1CC4H = T1CC2H = 0;  
	}
	else if (value < 300)
	{
		float maxDiff = 300.0f;
		float diff = value;

		uint16 v = (uint16)((float)0xfffe * diff / maxDiff);

		T1CC3L = T1CC1L = 0;
		T1CC3H = T1CC1H = 0;	

		T1CC4L = T1CC2L = (uint8)v;
		T1CC4H = T1CC2H = (uint8)(v >> 8);  
	}
	else
	{
		T1CC3L = T1CC1L = 0;
		T1CC3H = T1CC1H = 0;	
	
		T1CC4L = T1CC2L = 0;
		T1CC4H = T1CC2H = 0;  
		
		CarSetThrottle(lastThrottle);
	}  
	
	T1CNTL = 0;		
    
  
  
  
  /*
	if (value < 150)
	{
		M1_1 = M2_1 = 0;
		M1_2 = M2_2 = 1;
	}
	else if (value > 550)
	{
		M1_1 = M2_1 = 1;
		M1_2 = M2_2 = 0;
	}
	else
	{
		M1_1 = M2_1 = 0;
		M1_2 = M2_2 = 0;
		
		CarSetThrottle(lastThrottle);
	} */
};

void CarConnected()
{
/*	P2_2 = P2_1 = P2_0 = P1_7 = 0x01;

	CarLameReset();*/
  
  	connected = true;
  
//	P1_6 = 0;
	
	CarReset();

	osal_start_timerEx(mainTaskId, RC_UPDATE_STATUS_EVT, 5000);
	//osal_start_timerEx(mainTaskId, CAR_CONTROL_PERIODIC_EVT, 1);	

  
};

void CarDisConnected()
{
//	P2_2 = P2_1 = P2_0 = P1_7 = 0x00;

//	CarLameReset();
  
  	connected = false;
  
	CarReset();
	
	P1_6 = 1;

  
};

void CarUpdateStatus()
{
/*	HalAdcSetReference(HAL_ADC_REF_125V);

	uint16 adc = HalAdcRead(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_10);
  
	bleBatteryVoltage = (uint16)((float)adc * 10.0 * 1.25f * 3.0f / 511.0f);*/
};

void CarTick()
{
};

void CarSetLights(uint16 value)
{
	/*if ((value & 1) > 0)
	{
		P2_2 = P2_1 = P2_0 = P1_7 = 0x01;
	}
	else
	{
		P2_2 = P2_1 = P2_0 = P1_7 = 0x00;
	}*/
};

#endif
