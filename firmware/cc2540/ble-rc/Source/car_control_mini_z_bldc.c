#if defined(FIRMWARE_TYPE_MINIZBLDC)

#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "hci.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "blercprofile.h"
#include "hal_adc.h"

#define PWM_PERIODIC_EVT_PERIOD  20
#define SERVO_UPDATE_EVT_PERIOD  1

#define THROTTLE_CHANNEL_L		T1CC3L
#define THROTTLE_CHANNEL_H		T1CC3H

DECLARE_BATTERY_CONNECT(P0DIR |= (1 << 5), P0SEL &= ~(1 << 5), P0_5, 1);
DECLARE_LIGHTS(P2DIR |= (1 << 2) | (1 << 1); P1DIR |= (1 << 3) | (1 << 5) | (1 << 4), P2SEL &= ~((1 << 2) | (1 << 1)); P1SEL &= ~((1 << 3) | (1 << 5) | (1 << 4)), P2_2, P2_1, P1_3, P1_5, P1_4, true);
uint8* defaultName = "Mini-Z BLDC\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint8 deviceType = 0x01;
bool keepRunninOnNoCommands = false;

static uint16 mainTaskId           = 0;
static uint16 desiredServoPosition = 1020;
static bool   servoAdjusting       = false;

void CarSetDefaultValues()
{
	settings.pulseWidth			= 20;

	settings.throttleMin		= 218;
	settings.throttleMax		= 482;
	settings.throttleCenter	= 351;

	settings.steeringMin		= 666;
	settings.steeringMax		= 1446;
	settings.steeringCenter	= 1073;

	settings.batteryCalibration	= 0xa000;

	settings.mainLightNo		= 0;
	settings.backLightNo		= 1;
	settings.reverseLightNo	= 2;
	settings.leftLightNo		= 3;
	settings.rightLightNo		= 4;
	
	
	settings.batteryMaxVoltage	= 5000;
	settings.batteryMinVoltage	= 4000;
	settings.currentLimit				= 5000;
	settings.batteryCapacity		= 750;
	
	settings.deviceCapabilities	= HasLights;
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

static void SetP20PWM(int dutyCycle)
{
	if (dutyCycle <= 0)
	{
		T4CTL = 0x00; // stop timer
		
		P2DIR |= (1 << 0);
		P2SEL &= ~(1 << 0);
		P2_0 = 0;
	}
	else
	{
		PERCFG |= (1 << 4);  // Timer 4 on alternative 2 location		
		P2DIR |= (1 << 0);
		P2SEL |= (1 << 0);
		
		T4CC0 = 0xff - dutyCycle;
		T4CCTL0 = 0x1C;
		T4CTL = 0xF0; // start, free running 0-255, tick/128
	}
};

static void SetP16PWM(int dutyCycle)
{
	if (dutyCycle <= 0)
	{
		T3CTL = 0x00; // stop timer
		
		P1DIR |= (1 << 6);
		P1SEL &= ~(1 << 6);
		P1_6 = 0;
	}
	else
	{
		PERCFG |= (1 << 5);  // Timer 3 on alternative 2 location		
		P1DIR |= (1 << 6);
		P1SEL |= (1 << 6);

		T3CC0 = 0xff - dutyCycle;
		T3CCTL0 = 0x1C;
		T3CTL = 0xF0; // start, free running 0-255, tick/128
	}
};

void CarInit(uint8 taskId)
{
	mainTaskId = taskId;

 	P0SEL = 0x00;
	P0DIR = 0x00;
	P0 = 0x00;

 	P1SEL = 0x00;
	P1DIR = 0x00;
	P1 = 0x00;

 	P2SEL = 0x00;
	P2DIR = 0x00;
	P2 = 0x00;
	
	SetP20PWM(0);
	SetP16PWM(0);
	
	InitPWMTimers();
	CarReset();

	desiredServoPosition = settings.steeringCenter;
};

void CarSetSteering(uint16 value)
{
//	control.steering = value;
	
	desiredServoPosition = value;
};

void SetStatusConnected()
{
};

void CarSetThrottle(uint16 value)
{
	control.throttle = value;
};

void CarConnected()
{
	CarReset();
//	CarTick();
	SetStatusConnected();
	
	CarPPMTick();
};

void CarDisConnected()
{
	CarReset();
	SetStatusConnected();
//	CarTick();
	
	
//		P2DIR |= (1 << 0);
//	P1DIR |= (1 << 6);

//	P2SEL &= ~(1 << 0);	
	
	
	SetP20PWM(0);
	SetP16PWM(0);
};

void CarTick()
{
	
	//if (true == stats.connected)
	{
		osal_start_timerEx(mainTaskId, PERIODIC_EVENT, PWM_PERIODIC_EVT_PERIOD);
	}

	THROTTLE_CHANNEL_L = (uint8)control.throttle;
	THROTTLE_CHANNEL_H = (uint8)(control.throttle >> 8);

	// Make sure timer will count continously
	HCI_EXT_ClkDivOnHaltCmd(HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT);

	#if defined (POWER_SAVING)
		osal_pwrmgr_device(PWRMGR_ALWAYS_ON);
	#endif

	// Reset timer
	T1CNTL = 0;

	// Start timer in modulo mode.
	T1CTL |= 0x02;
};

void CarUpdateStatus()
{
	HalAdcSetReference(HAL_ADC_REF_125V);
	
	uint16 battery = HalAdcRead(HAL_ADC_CHN_AIN1, HAL_ADC_RESOLUTION_10);

	battery = 1250 * battery * 2 / 1023;
	
	stats.batteryVoltage = battery;
};

////// servoi/////////////////////////////

static int lastServoMotorSpeed = 0;
static long servoError[10] = { 0 };
static long servoAccumulator = 0;

//static uint16 actualServoPosition = 0;






void UpdateError(uint16 currentPosition)
{
  /*
	if (fabs(currentPosition - actualServoPosition) > 10) //2)//3)
	{
		actualServoPosition = currentPosition;
	}
	else
	{
		actualServoPosition = currentPosition;
	}
      */
	// shift error values
//	for (int i = 0; i < 10; i++)
//	{
	//	servoError[i + 1] = servoError[i];
//	}

  for (int i = 9; i > 0; i--)
  {
	servoError[i] = servoError[i - 1];
  }

	// load new error into top array spot
	servoError[0] = (long)desiredServoPosition - (long)currentPosition;//actualServoPosition;
};

int CalculatePID()
{
	float pTerm = 255.0f / 850.0f;
	int iTerm = 0;//25;
	int dTerm = 0;
	uint8 divider = 1; //12; //11; //10;

	// Calculate the PID
	float pid = servoError[0] * pTerm;     // start with proportional gain
	//servoAccumulator += servoError[0];  // accumulator is sum of errors
	//pid += iTerm * servoAccumulator; // add integral gain and error accumulation
	//pid += dTerm * (servoError[0] - servoError[9]); // differential gain comes next
	//pid = pid >> divider; // scale PID down with divider

	if (pid >= 255)
	{
		pid = 255;
	}
	else if (pid <= -255)
	{
		pid = -255;
	}

	return -pid;
};

void SetServoMotorSpeed(int speed)
{
	if (speed > 10)
	{
		//P2_0 = 0;
		SetP20PWM(0);
		//P1_6 = 1;
		SetP16PWM(speed);
	}
	else if (speed < -10)
	{
		//P1_6 = 0;
		SetP16PWM(0);
		//P2_0 = 1;
		SetP20PWM(speed * -1);
	}
	else
	{
		//P1_6 = 0;
		SetP16PWM(0);
		//P2_0 = 0;
		SetP20PWM(0);
	}
};

void MoveServoMotor(int value)
{
//	if (lastServoMotorSpeed != value)
	{
		lastServoMotorSpeed = value;
		SetServoMotorSpeed(value);
	}
};

void UpdateServo(uint16 potPosition)
{
	stats.servoPosition = potPosition;
	UpdateError(potPosition);
	MoveServoMotor(CalculatePID());
};

void AdjustServo()
{
	if (false == servoAdjusting)
	{
		servoAdjusting = true;

		HalAdcSetReference(HAL_ADC_REF_AVDD);
		UpdateServo(HalAdcRead(HAL_ADC_CHN_AIN0, HAL_ADC_RESOLUTION_12));

		servoAdjusting = false;
	}
};

void CarPPMTick()
{
	if (true == stats.connected)
	{
		// Restart timer servo update
		osal_start_timerEx(mainTaskId, PPM_EVENT, SERVO_UPDATE_EVT_PERIOD);
		AdjustServo();
	}
};

void FinishAccelerometerUpdate()
{
};

void TempFinishUpdate()
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
