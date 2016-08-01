#if defined(FIRMWARE_TYPE_DNANO)

#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "hci.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "blercprofile.h"
#include "car_control.h"
#include "hal_adc.h"
#include "math.h"

#define SERVO_UPDATE_INTERVAL 1

DECLARE_BATTERY_CONNECT(P1DIR |= (1 << 5), P1SEL &= ~(1 << 6), P1_5, 0);
DECLARE_LIGHTS(;, ;, P2_2, P2_1, P0_2, P0_7, P1_7, false); 
uint8* defaultName = "dNano\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint8 deviceType = 0x02;
bool keepRunninOnNoCommands = false;

static float maxValue = 700.0f;
static uint16 lastThrottleSpeed = 350;
//static int lastServoMotorSpeed = 0;
static long servoError[10] = { 0 };
//static long servoAccumulator = 0;
static uint8 mainTaskId = 0;


void CarSetDefaultValues()
{
	settings.throttleMin        = 0;
	settings.throttleMax        = 700;
	settings.throttleCenter     = 350;

	settings.steeringMin        = 666;
	settings.steeringMax        = 1446;
	settings.steeringCenter     = 1073;

	settings.batteryMaxVoltage  = 5000;
	settings.batteryMinVoltage  = 4000;
	settings.currentLimit       = 5000;
	settings.batteryCapacity    = 750;
	settings.deviceCapabilities = 0;
};
    
void CarReset()
{
	// no connection no pwm..	
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
	
	//IEN0 |= 0x02; // ADC interupt
	
	control.steering = settings.steeringCenter;
	
	P1DIR |= (1 << 6);
	P1_6 = 1;
};

void CarSetSteering(uint16 value)
{
	control.steering = value;
};

void CarSetThrottle(uint16 value)
{
	if (value == lastThrottleSpeed)
	{
		return;
	}
  
	lastThrottleSpeed = value;
	
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

void CarConnected()
{
	CarReset();
	
	osal_start_timerEx(mainTaskId, PERIODIC_EVENT, 1);
        
	P2SEL &= ~((1 << 2) | (1 << 1));
	P2DIR |= (1 << 2) | (1 << 1);
	
	P2_2 = 1;
};

void CarDisConnected()
{
	CarReset();

	P2_2 = 0;
};

void CarUpdateStatus()
{
	stats.engineTemperature = stats.mcuTemperature;
};

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
 
	for (int i = 9; i > 0; i--)
	{
		servoError[i] = servoError[i - 1];
	}

	// load new error into top array spot  
	servoError[0] = (long)control.steering - (long)currentPosition;//actualServoPosition;
};
  
int CalculatePID()
{
	float pTerm = 255.0f / 850.0f;
	//int iTerm = 0;//25;
	//int dTerm = 0;
	//uint8 divider = 1; //12; //11; //10;

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

	return (int)-pid;
};
  
void SetServoMotorSpeed(int speed)
{
	float invertedSpeed = (uint16)(10.0 + 245.0 * (255.0f - (float)fabs(speed)) / 255.0f);
	float maxDiff = 255.0f;
  
	if (speed > 20) //50)
	{
		uint16 v = (uint16)((float)0xfffe * invertedSpeed / maxDiff);

		T1CC3L = (uint8)v;
		T1CC3H = (uint8)(v >> 8);	
	
		T1CC4L = 0;
		T1CC4H = 0;		
	}
	else if (speed < -20) //-50)
	{
		uint16 v = (uint16)((float)0xfffe * invertedSpeed / maxDiff);

		T1CC3L = 0;
		T1CC3H = 0;	
	
		T1CC4L = (uint8)v;
		T1CC4H = (uint8)(v >> 8);		
	}
	else
	{
		T1CC4L = 0;
		T1CC4H = 0;	
	
		T1CC3L = 0;
		T1CC3H = 0;
	}
};

void MoveServoMotor(int value)
{
//	if (lastServoMotorSpeed != value)
	{
		//lastServoMotorSpeed = value;
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
	if (false == stats.adjustingServo)
	{
		stats.adjustingServo = true;

		HalAdcSetReference(HAL_ADC_REF_AVDD);
		UpdateServo(HalAdcRead(HAL_ADC_CHN_AIN0, HAL_ADC_RESOLUTION_12));
	
		stats.adjustingServo = false;
	}
};

void CarTick()
{
	if (true == stats.connected)
	{
		osal_start_timerEx(mainTaskId, PERIODIC_EVENT, SERVO_UPDATE_INTERVAL);
		AdjustServo();
	}
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
