#include "comdef.h"
#include "bcomdef.h"
#include "gatt.h"
#include "blercprofile.h"
#include "car_control.h"
#include "hal_adc.h"
#include "att.h"
#include "gapgattserver.h"
#include "battservice.h"
#include "racecar.h"
#include "osal_snv.h"
#include "gapbondmgr.h"

#define DEVICE_SETTINGS_FLASH_ADDRESS  0x90
#define DEVICE_SETTINGS_COUNT          18
#define STATUS_UPDATE_EVENT_PERIOD     5000

Settings settings;

void BaseCarInit(uint8 taskId, uint8* deviceScanName)
{
	settings.deviceScanNameAddress = deviceScanName;

	stats.mainTaskId = taskId;
  
	BaseCarLoadValues();

	LoadDeviceName(settings.deviceScanNameAddress);
	
	CarInit(taskId);
	CarInitBattery();
	CarInitLights();
};

void BaseCarConnected(uint8 taskId)
{
	stats.connected = true;

	CarConnectBattery();
	  
	CarSetLight(settings.reverseLightNo, 0x00);
	CarSetLight(settings.leftLightNo, 0x00);
	CarSetLight(settings.rightLightNo, 0x00);
	CarSetLight(settings.mainLightNo, 0x01);
	CarSetLight(settings.backLightNo, 0x01);	  
	  
	CarConnected(taskId);

	stats.settingsChanged = 0;

	osal_start_timerEx(stats.mainTaskId, UPDATE_STATUS_EVENT, STATUS_UPDATE_EVENT_PERIOD);
};

void BaseCarDisConnected()
{
	stats.connected = false;

	CarDisConnected();

	if (true == stats.settingsChanged)
	{
		BaseCarSaveValues();
	}
	
	BaseCarSetLights(0x0000);
	
	CarDisConnectBattery();
};

void BaseCarUpdateMcuTemperature()
{
	HalAdcSetReference(HAL_ADC_REF_125V);

	TR0 |= 0x01; // TR0 must be modified prior to ATEST,
	ATEST |= 0x01; // otherwise P0_0 and P0_1 forced with 0

	uint16 t = HalAdcRead(HAL_ADC_CHN_TEMP, HAL_ADC_RESOLUTION_12);

	TR0 &= ~0x01;
	ATEST &= ~0x01;

	float analogTemperature = (((float)t * 10.0f * 1250.0f / 2047.0f) * 10.0f / 45.0f - 1490.0f) / 2.0f;

	stats.mcuTemperature = (uint16)(analogTemperature * 10.0f);
};

void BaseCarUpdateStatus()
{
	BaseCarUpdateMcuTemperature();

	CarUpdateStatus();
	
	Batt_MeasLevel();

	if (true == stats.connected)
	{
		osal_start_timerEx(stats.mainTaskId, UPDATE_STATUS_EVENT, STATUS_UPDATE_EVENT_PERIOD);
	}
};

bool HasCapability(uint16 capability)
{
	return ((settings.deviceCapabilities & capability) > 0);
};

void BaseCarSaveValues()
{
	if (SUCCESS == osal_snv_write(DEVICE_SETTINGS_FLASH_ADDRESS, sizeof(uint16) * DEVICE_SETTINGS_COUNT, &settings))
	{
	}
};

static void BaseCarLoadValues()
{
	if (SUCCESS == osal_snv_read(DEVICE_SETTINGS_FLASH_ADDRESS, sizeof(uint16) * DEVICE_SETTINGS_COUNT, &settings))
	{
	}
	else
	{
		settings.pulseWidth         = 20;
		settings.throttleMin        = 275;
		settings.throttleMax        = 475;
		settings.throttleCenter     = 375;
		settings.steeringMin        = 275;
		settings.steeringMax        = 475;
		settings.steeringCenter     = 375;
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
		settings.deviceCapabilities = 0;	  
	  
		CarSetDefaultValues();
	}
};

static void BaseCarSetLightState(uint16 lightsState, uint16 bit)
{
	uint16 b = 1 << bit;
  
	if ((lightsState & b) > 0)
	{
		CarSetLight(bit, 1);
	}
	else
	{
		CarSetLight(bit, 0);
	}
};

void BaseCarSetLights(uint16 value)
{
	BaseCarSetLightState(value, 0);
	BaseCarSetLightState(value, 1);
	BaseCarSetLightState(value, 2);
	BaseCarSetLightState(value, 3);
	BaseCarSetLightState(value, 4);
};

void BaseCarStartNoDataTimeOut()
{
	osal_stop_timerEx(stats.mainTaskId, NO_COMMAND_EVENT);
	osal_start_timerEx(stats.mainTaskId, NO_COMMAND_EVENT, 500);
};

void BaseCarOnNoCommand()
{
	if (false == keepRunninOnNoCommands)
	{
		CarSetThrottle(settings.throttleCenter);
		CarSetSteering(settings.steeringCenter);
	}
};
