#if defined (FIRMWARE_TYPE_ESCSERVO2)

#include "car_control.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"
#include "hci.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "blercprofile.h"
#include "hal_ina220.h"
#include "hal_ds18b20.h"
#include "hal_acc.h"
#include "accelerometerservice.h"
#include "gattservapp.h"

#define PWM_PERIODIC_EVT_PERIOD 20
#define ACC_DEFAULT_PERIOD      1000
#define ACC_FSM_PERIOD          20
#define ST_CFG_SENSOR_DISABLE   0x00
#define ST_CFG_SENSOR_ENABLE    0x01
#define ST_CFG_CALIBRATE        0x02
#define ST_CFG_ERROR            0xFF

DECLARE_BATTERY_CONNECT(P1DIR |= 0x01, P1SEL &= ~(0x01), P1_0, 1);
DECLARE_LIGHTS(P2DIR |= 0x02; P1DIR |= (1 << 1) | (1 << 4) | (1 << 7); P0DIR |= (1 << 4), P2SEL &= ~(0x2); P1SEL &= ~((1 << 1) | (1 << 4) | (1 << 7)); P0SEL &= ~(1 << 4), P1_1, P0_4, P1_4, P1_7, P2_1, FALSE);
uint8* defaultName = "Radio Controlled Car\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint8 deviceType = 0x04;
bool keepRunninOnNoCommands = false;
static uint16 mainTaskId = 0;
static bool accelerometerEnabled = true;
static uint16 sensorAccelerometerPeriod = ACC_DEFAULT_PERIOD;

#if defined(RC_BUG)
	#define STEERING_CHANNEL_L		T1CC4L
	#define STEERING_CHANNEL_H		T1CC4H
	#define THROTTLE_CHANNEL_L		T1CC1L
	#define THROTTLE_CHANNEL_H		T1CC1H
	#define CONNECTED_LED_PIN		P0_2
	DECLARE_BATTERY_CONNECT(P0DIR |= (1 << 5), P0SEL &= ~(1 << 5), P0_5, 1);
	DECLARE_LIGHTS(P2DIR |= (1 << 2) | (1 << 1); P1DIR |= (1 << 6) | (1 << 5) | (1 << 4), P2SEL &= ~((1 << 2) | (1 << 1)); P1SEL &= ~((1 << 6) | (1 << 5) | (1 << 4)), P2_2, P2_1, P1_6, P1_5, P1_4);
#else
	#define STEERING_CHANNEL_L T1CC4L
	#define STEERING_CHANNEL_H T1CC4H
	#define THROTTLE_CHANNEL_L T1CC3L
	#define THROTTLE_CHANNEL_H T1CC3H
	#define CONNECTED_LED_PIN  P0_2
#endif

static void AccelerometerChangeCallBack(uint8 parameterId)
{
	uint8 newValue;

	switch (parameterId)
	{
		case ACCELEROMETER_CONF:
			Accel_GetParameter(ACCELEROMETER_CONF, &newValue);
			if (newValue == ST_CFG_SENSOR_DISABLE)
			{
				// Put sensor to sleep
				if (accelerometerEnabled)
				{
					accelerometerEnabled = FALSE;
					osal_set_event(stats.mainTaskId, ACCELEROMETER_SENSOR_EVENT);
				}
			}
			else if (newValue == ST_CFG_SENSOR_ENABLE)
			{
				if (false == accelerometerEnabled)
				{
					accelerometerEnabled = TRUE;
					osal_set_event(stats.mainTaskId, ACCELEROMETER_SENSOR_EVENT);
				}
			}
			break;
		case ACCELEROMETER_PERI:
			Accel_GetParameter(ACCELEROMETER_PERI, &newValue);
			sensorAccelerometerPeriod = newValue * ACCELEROMETER_TIME_UNIT;
			break;
		default:
			// Should not get here
			break;
	}
};

static accelCBs_t AccelerometerCallBacks =
{
	AccelerometerChangeCallBack
};

static void ReadAccelerometerData()
{
	uint8 data[ACCELEROMETER_DATA_LEN];

	if (HalAccRead(data))
	{
		Accel_SetParameter(ACCELEROMETER_DATA, ACCELEROMETER_DATA_LEN, data);
	}
};

static void ResetCharacteristicValue(uint16 serviceUuid, uint8 parameterId, uint8 value, uint8 parameterLength)
{
	uint8* data = osal_mem_alloc(parameterLength);

	if (data == NULL)
	{
		return;
	}

	osal_memset(data, value, parameterLength);

	switch(serviceUuid)
	{
		case ACCELEROMETER_SERV_UUID:
			Accel_SetParameter(parameterId, parameterLength, data);
			break;
		default:
			break;
	}

	osal_mem_free(data);
};

static void ResetCharacteristicValues()
{
	ResetCharacteristicValue(ACCELEROMETER_SERV_UUID, ACCELEROMETER_DATA, 0, ACCELEROMETER_DATA_LEN);
	ResetCharacteristicValue(ACCELEROMETER_SERV_UUID, ACCELEROMETER_CONF, ST_CFG_SENSOR_DISABLE, sizeof(uint8));
	ResetCharacteristicValue(ACCELEROMETER_SERV_UUID, ACCELEROMETER_PERI, ACC_DEFAULT_PERIOD / ACCELEROMETER_TIME_UNIT, sizeof(uint8));
};

void FinishAccelerometerUpdate()
{
	if (accelerometerEnabled)
	{
		ReadAccelerometerData();
		osal_start_timerEx(stats.mainTaskId, ACCELEROMETER_SENSOR_EVENT, sensorAccelerometerPeriod);
	}
	else
	{
		ResetCharacteristicValues();
	}
};

void TempFinishUpdate()
{
	HalDS18B20FinishReading(&stats.engineTemperature, NULL);
};

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
	#if defined(RC_BUG)
	 	uint8 pwmPins = 0x40 | 0x08; // P0.3 and P0.6 as PWM

		P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
		P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

		PERCFG |= 0x03;             //  Move T1 to alternate2 location   ----Move USART1&2 to alternate2 location so that T1 is visible
	#else
		uint8 pwmPins = 0x80 | 0x40; // P0.6 and P0.7 as PWM

		P0DIR |= pwmPins;	// Data direction OUT for the PWM pins
		P0SEL |= pwmPins;	// Choose peripheral mode for PWM pins

		PERCFG |= 0x40; //0x03;             //  Move T1 to alternate2 location   ----Move USART1&2 to alternate2 location so that T1 is visible
	#endif
	
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

static void SetStatusConnected()
{
	#if !defined(RC_BUG)
		CONNECTED_LED_PIN = 0x01;
	#endif
};

static void SetStatusDisConnected()
{
	#if !defined(RC_BUG)
		CONNECTED_LED_PIN = 0x00;
	#endif
};

static void InitStatusLed()
{
	#if !defined(RC_BUG)
		P0SEL &= ~0x04;
		P0DIR |= 0x04;
	
		SetStatusDisConnected();
	#endif
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
	
	InitPWMTimers();
	CarReset();
	InitStatusLed();
	
	HalIna220Init(0xffff);
	HalDS18B20Init();
	HalAccInit();
	HalAccTest();
	
	Accel_AddService(GATT_ALL_SERVICES);
	Accel_RegisterAppCBs(&AccelerometerCallBacks);	
};

void CarSetSteering(uint16 value)
{
	control.steering = value;
};

void CarSetThrottle(uint16 value)
{
	control.throttle = value;
};

void CarConnected()
{
	CarReset();
	CarTick();
	SetStatusConnected();
	PowerOnIna220();
};

void CarDisConnected()
{
	CarReset();
	SetStatusConnected();
	CarTick();
	PowerOffIna220();
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

	// Start timer in modulo mode.
	T1CTL |= 0x02;
};

void CarUpdateStatus()
{
	HalDS18B20StartReading(stats.mainTaskId);
	
	HalIna220UpdateVoltage(&stats.batteryVoltage);
	HalIna220UpdateCurrent(&stats.batteryCurrent);	

	ReadAccelerometerData();
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
