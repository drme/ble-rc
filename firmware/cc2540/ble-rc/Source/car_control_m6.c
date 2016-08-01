#if defined(FIRMWARE_TYPE_M6)

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

DECLARE_BATTERY_CONNECT(P1DIR |= (1 << 6), P1SEL &= ~(1 << 6), P1_6, 0);

#if defined(PWM)
	DECLARE_LIGHTS(P0DIR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 7), P0SEL &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 7)), P0_0, P0_1, P0_2, P0_7, P1_0, FALSE); 
#elif defined(M6)
	DECLARE_LIGHTS(P2DIR |= (1 << 0) | (1 << 1) | (1 << 2); P1DIR |= (1 << 7);, P2SEL &= ~((1 << 0) | (1 << 1) | (1 << 2)); P1SEL &= ~(1 << 7), P2_2 = P2_1 = P2_0 = P1_7, P0_0, P0_1, P0_2, P0_3); 
#endif

/*
//#define	IN1					P2_0
//#define	IN2					P0_3

//#define IN3					P1_4
//#define IN4					P1_2
	
#if defined(V2)
	#define STEERING_LEFT_PIN	P1_0
	#define STEERING_RIGHT_PIN	P1_1

	#define H1	IN3
	#define H2	IN4
#elif defined(V1)
	#define STEERING_LEFT_PIN	P1_0
	#define STEERING_RIGHT_PIN	P1_1

	#define H1	IN1
	#define H2	IN2
#elif defined(V3)
	#define H1	IN3
	#define H2	IN4

	#define STEERING_LEFT_PIN	IN1
	#define STEERING_RIGHT_PIN	IN2
#elif defined(V4)
	#define H1	IN3
	#define H2	IN4

	#define STEERING_LEFT_PIN	IN1 = P1_0 = GREEN_LED
	#define STEERING_RIGHT_PIN	IN2 = P1_1 = RED_LED
#else
	//#error "Undefined configuration"
#endif

#endif

*/

#if defined(M6)
	#define GREEN_LED			P0_0
	#define RED_LED				P0_1
	#define H1					P0_6
	#define H2					P0_7
	#define STEERING_LEFT_PIN	P1_0
	#define STEERING_RIGHT_PIN	P1_1
#elif defined(PWM)
	#define STEERING_LEFT_PIN	P0_3
	#define STEERING_RIGHT_PIN	P0_4
#else
	#error "Undefined configuration"
#endif

static uint8 mainTaskId = 0;
static bool releaseBreak = false;

void CarSetDefaultValues()
{
	settings.throttleMin		= 160;
	settings.throttleMax		= 590;
	settings.throttleCenter		= 375;
	settings.steeringMin		= 250;
	settings.steeringMax		= 500;
	settings.steeringCenter		= 375;
	settings.deviceCapabilities	= IsSimple | HasLights;
};

static void CarReset()
{
	STEERING_RIGHT_PIN	= 0x0;
	STEERING_LEFT_PIN	= 0x0;

	#if defined(M6)
		H1			= 0x0;
		H2			= 0x0;
		GREEN_LED	= 0x1;
		RED_LED		= 0x1;
	#elif defined(PWM)
		T1CC4L = 0;
		T1CC4H = 0;	
		T1CC3L = 0;
		T1CC3H = 0;
		T1CC1L = 0;
		T1CC1H = 0;	
		T1CC2L = 0;
		T1CC2H = 0;
		T1CNTL = 0;		
	#else
		#error "Undefined configuration"
	#endif
};

static void CarRCInitPWMTimers()
{
	uint8 pwmPins = (1 << 5) | (1 << 6); // P0.5, P0.6 as PWM

	P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
	P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

	PERCFG |= 0x03;     //  Move USART1&2 to alternate2 location so that T1 is visible
  
	T1CC0L = 0xFF;
	T1CC0H = 0xff;

	T1CCTL0 = (T1CCTL0 & ~0x23) | 0x1C; // Channel 0 in compare mode, Set output on compare-up, clear on 0 (50% duty cycle)	

//	T1CC1L = 0x1 * 0;  // PWM duty cycle, Channel 1 (P0_3)
//	T1CC1H = 0x0;
//	T1CCTL1 = (T1CCTL1 & ~0x23) | 0x1C; // Channel 1 in compare mode, Set output on compare-up, clear on compare-down
   
//	T1CC2L = 0x1 * 0;  // PWM duty cycle, Channel 2 (P0_4)
//	T1CC2H = 0x00;
//	T1CCTL2 = (T1CCTL2 & ~0x23) | 0x1C;  // Channel 2 in compare mode, Set output on compare-up, clear on compare-down

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
	mainTaskId = taskId;
  
  	P0SEL = 0x00;
	P0DIR = 0xff;
	P0 = 0x00;
  
  	P1SEL = 0x00;
	P1DIR = 0xff;
	P1 = 0x00;

	P2SEL &= !((1 << 0) | (1 << 1) | (1 << 2));
	P2DIR |= (1 << 0) | (1 << 1) | (1 << 2);
	P2_0 = P2_1 = P2_2 = 0x00;
 
	#if defined(PWM)
		CarRCInitPWMTimers();
	#elif defined(M6)
	#else
		#error "Undefined configuration"
	#endif
		
	CarReset();
};

void CarSetSteering(uint16 value)
{
	if (value < 150)
	{
		STEERING_LEFT_PIN	= 0;
		STEERING_RIGHT_PIN	= 1;
	}
	else if (value > 550)
	{
		STEERING_LEFT_PIN	= 1;
		STEERING_RIGHT_PIN	= 0;
	}
	else
	{
		STEERING_LEFT_PIN	= 0;
		STEERING_RIGHT_PIN	= 0;
	}
};

void CarSetThrottle(uint16 value)
{
	#if defined(M6)
		if (value < 350)
		{
			H1 = 1;
			H2 = 0;
			releaseBreak = false;
		}
		else if (value > 400)
		{
			H1 = 0;
			H2 = 1;
			releaseBreak = false;
		}
		else
		{
			if (false == releaseBreak)
			{
				H1 = 1;
				H2 = 1;
				releaseBreak = true;
				osal_start_timerEx(mainTaskId, CAR_CONTROL_PERIODIC_EVT, 10);
			}
		}
	#elif defined(PWM)
  		static float maxValue = 700.0f;
  
	  	if (value > 450)
		{
			float maxDiff = maxValue - 450.0f;
			float diff = maxValue - value;
	  
			uint16 v = (uint16)((float)0xfffe * diff / maxDiff);
		
			T1CC3L = (uint8)v;
			T1CC3H = (uint8)(v >> 8);	

			T1CC4L = 0;
			T1CC4H = 0;  
		}
		else if (value < 300)
		{
			float maxDiff = 300.0f;
			float diff = value;

			uint16 v = (uint16)((float)0xfffe * diff / maxDiff);

			T1CC3L = 0;
			T1CC3H = 0;	

			T1CC4L = (uint8)v;
		    T1CC4H = (uint8)(v >> 8);  
		}
		else
		{
			T1CC3L = 0;
			T1CC3H = 0;	
	
			T1CC4L = 0;
			T1CC4H = 0;  
		}  

		T1CNTL = 0;		
	#else
		#error "Undefined configuration"
	#endif
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
	HalAdcSetReference(HAL_ADC_REF_125V);

	uint16 adc = HalAdcRead(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_10);
  
	stats.batteryVoltage = (uint16)((float)adc * 10.0 * 1.25f * 3.0f / 511.0f);
};

void CarTick()
{
	if (true == releaseBreak)
	{
	#if defined(PWM)
	#elif defined(M6)
		H1 = 0;
		H2 = 0;
	#else
		#error "Undefined configuration"
	#endif		
		releaseBreak = false;
	}
};

void CarPPMTick()
{
};

#endif
