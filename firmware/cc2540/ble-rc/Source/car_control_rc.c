#if defined(FIRMWARE_TYPE_ESCSERVO)

#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "hci.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "blercprofile.h"
#include "hal_ina226.h"

#define PWM_PERIODIC_EVT_PERIOD 20
#define STEERING_CHANNEL_L      T1CC4L
#define STEERING_CHANNEL_H      T1CC4H
#define THROTTLE_CHANNEL_L      T1CC3L
#define THROTTLE_CHANNEL_H      T1CC3H

DECLARE_BATTERY_CONNECT(P0DIR |= (1 << 5), P0SEL &= ~(1 << 5), P0_5, 1);
DECLARE_LIGHTS(P2DIR |= 0x07; P1DIR |= (1 << 6); P0DIR |= (1 << 5), P2SEL &= ~(0x7); P1SEL &= ~(1 << 6); P0SEL &= ~(1 << 5), P2_2, P2_1, P1_6, P2_0, P0_5, FALSE);
uint8* defaultName = "Radio Controlled Car\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint8 deviceType = 0x04;
bool keepRunninOnNoCommands = false;
static bool ticking = false;

static uint16 mainTaskId = 0;

void CarSetDefaultValues()
{
	settings.pulseWidth         = 20;
	settings.throttleMin        = 345;
	settings.throttleMax        = 422;
	settings.throttleCenter     = 400;
	settings.steeringMin        = 250;
	settings.steeringMax        = 500;
	settings.steeringCenter     = 335;
	settings.batteryMaxVoltage  = 7200;
	settings.batteryMinVoltage  = 5000;
	settings.currentLimit       = 5000;
	settings.batteryCapacity    = 5000;
	settings.batteryCalibration = 0xa000;
	settings.mainLightNo        = 0;
	settings.backLightNo        = 1;
	settings.reverseLightNo     = 2;
	settings.leftLightNo        = 3;
	settings.rightLightNo       = 4;		
	settings.deviceCapabilities = HasLights;
};

static void CarReset()
{
	control.throttle        = settings.throttleCenter;
	control.steering        = settings.steeringCenter;
	control.lights          = 0;
	stats.batteryVoltage    = 0;
	stats.batteryCurrent    = 0;
	stats.batteryUsed       = 0;
	stats.engineTemperature = 0;
	stats.mcuTemperature    = 0;
};

static void InitPWMTimers()
{
	uint8 pwmPins = 0x80 | 0x40; // P0.6 and P0.7 as PWM

	P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
	P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

	PERCFG |= 0x40; //0x03;             //  Move T1 to alternate2 location   ----Move USART1&2 to alternate2 location so that T1 is visible
	
	// Initialize Timer 1
	T1CTL = 0x0C;		// Div = 128, CLR, MODE = Suspended
	T1CCTL0 = 0x4C;		// IM = 1, CMP = Clear output on compare; Mode = Compare

	T1CC0H = 0x02;		// Ticks = 600 (2.4ms)
	T1CC0L = 0x58;		// Ticks = 600 (2.4ms)

	T1CCTL3 = 0x0C;		// IM = 0; CMP = Clear output on compare; Mode = Compare
	T1CCTL4 = 0x0C;		// IM = 0; CMP = Clear output on compare; Mode = Compare

	T1CCTL1 = 0x0C;		// IM = 0; CMP = Clear output on compare; Mode = Compare
	T1CCTL2 = 0x0C;		// IM = 0; CMP = Clear output on compare; Mode = Compare

	T1CNTL = 0;			// Reset timer to 0;

	IEN1 |= 0x02;		// Enable T1 cpu interrupt

	// Set up the timer registers
	// Ticks = 375 (1,5ms initial duty cycle)
	CarReset();

	T1CTL ^= 0x02;		// timer is off initially
	
	CarTick();
};

void CarInit(uint8 taskId)
{
	mainTaskId = taskId;
	
	P0SEL = 0x00;
	P0DIR = 0x00;
	P0    = 0x00;
  
	P1SEL = 0x00;
	P1DIR = 0x00;
	P1    = 0x00;

	P2SEL = 0x00;
	P2DIR = 0x00;
	P2    = 0x00;
	
	HalIna226Init(15.0f, 0.01f);
	
	InitPWMTimers();
	CarReset();
};

void CarSetSteering(uint16 value)
{
	control.steering = value;
	ticking = true;
};

void CarSetThrottle(uint16 value)
{
	control.throttle = value;
	ticking = true;
};

void CarConnected()
{
	CarReset();
	CarTick();
	PowerOnIna226();
	ticking = false;
};

void CarDisConnected()
{
	CarReset();
	CarTick();
	PowerOffIna226();
	ticking = false;
};

void CarTick()
{
	if (true == stats.connected)
	{
		osal_start_timerEx(mainTaskId, PERIODIC_EVENT, PWM_PERIODIC_EVT_PERIOD);
	}

	STEERING_CHANNEL_L = (uint8)control.steering;
	STEERING_CHANNEL_H = (uint8)(control.steering >> 8);	

	THROTTLE_CHANNEL_L = (uint8)control.throttle;
	THROTTLE_CHANNEL_H = (uint8)(control.throttle >> 8);

	// Make sure timer will count continously
	HCI_EXT_ClkDivOnHaltCmd(HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT);

	#if defined (POWER_SAVING)
		osal_pwrmgr_device(PWRMGR_ALWAYS_ON);
	#endif

	// Reset timer
	T1CNTL = 0;

	if (true == ticking)
	{
		// Start timer in modulo mode.
		T1CTL |= 0x02;
	}
};

void CarUpdateStatus()
{
	HalIna226UpdateVoltage(&stats.batteryVoltage);
	HalIna226UpdateCurrent(&stats.batteryCurrent);
};

void CarPPMTick()
{
};

/**
 * ISR to reset timer after RC control pulse.
 */
#pragma register_bank=2
#pragma vector = T1_VECTOR
__interrupt void rcISR (void)
{
	uint8 flags = T1STAT;
		
   	// T1 ch 0
	if (flags & 0x01)
	{
		// Stop Timer 1
		T1CTL ^= 0x02;

		//Allow clock division and re-enable power saving until next pulse
		HCI_EXT_ClkDivOnHaltCmd(HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT);

		#if defined(POWER_SAVING)
			osal_pwrmgr_device(PWRMGR_BATTERY);
		#endif
	}

	T1STAT = ~ flags;
};

#endif
