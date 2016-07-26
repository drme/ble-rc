#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "blercprofile.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_adc.h"
#include "gatt.h"
#include "hci.h"
#include "osal_snv.h"
#include "car_control.h"
#include "gapgattserver.h"
#include "dl_util.h"
  
#define DECLARE_UUID(name, id)                     static CONST uint8 name[DL_UUID_SIZE] = { DL_UUID(id) };
#define DECLARE_ATTR_DESCRIPTION(description)      { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, description }
#define DECLARE_ATTR_VALUE(ID, permissions, value) { { DL_UUID_SIZE, ID }, permissions, 0, (uint8*)&value }
#define DECLARE_ATTR_PROPERTIES(properties)        { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &properties }
#define SET_VALUE(x)                               if (len == sizeof(uint16)) { x = *((uint16*)value); } else { ret = bleInvalidRange; };
#define DECLARE_ATTR(bleProperties, bleUUID, blePermission, bleValue, bleDescription)	DECLARE_ATTR_PROPERTIES(bleProperties), DECLARE_ATTR_VALUE(bleUUID, blePermission, bleValue), DECLARE_ATTR_DESCRIPTION(bleDescription)

#define DEVICE_NAME_FLASH_ADDRESS      0x91
#define DEVICE_PASSWORD_FLASH_ADDRESS  0x92

DECLARE_UUID(bleRCProfileServUUID,                    BLE_RC_SERVICE_UUID);
DECLARE_UUID(bleRCProfileCharThrottleUUID,            BLE_RC_THROTTLE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringUUID,            BLE_RC_STEERING_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharPulseWidthUUID,          BLE_RC_PULSE_WIDTH_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryVoltageUUID,      BLE_RC_BATTERY_VOLTAGE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharLightsUUID,              BLE_RC_LIGHTS_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharThrottleMinUUID,         BLE_RC_THROTTLE_MIN_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharThrottleMaxUUID,         BLE_RC_THROTTLE_MAX_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharThrottleCenterUUID,      BLE_RC_THROTTLE_CENTER_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringMinUUID,         BLE_RC_STEERING_MIN_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringMaxUUID,         BLE_RC_STEERING_MAX_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringCenterUUID,      BLE_RC_STEERING_CENTER_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryCurrentUUID,      BLE_RC_BATTERY_CURRENT_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryMaxVoltageUUID,   BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryCapacityUUID,     BLE_RC_BATTERY_CAPACITY_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryUsedUUID,         BLE_RC_BATTERY_USED_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharDeviceNameUUID,          BLE_RC_DEVICE_NAME_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharDevicePassUUID,          BLE_RC_DEVICE_PASS_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryCalibrrationUUID, BLE_RC_BATTERY_CALIBRATION_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharMainLightNoUUID,         BLE_RC_MAIN_LIGHT_NO_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBackLightNoUUID,         BLE_RC_BACK_LIGHT_NO_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharReverseLightNoUUID,      BLE_RC_REVERSE_LIGHT_NO_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharLeftLightNoUUID,         BLE_RC_LEFT_LIGHT_NO_CHAR_UUID	);
DECLARE_UUID(bleRCProfileCharRightLightNoUUID,        BLE_RC_RIGHT_LIGHT_NO_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharServoPosUUID,            BLE_RC_SERVO_POS_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharCurrentLimitUUID,        BLE_RC_CURRENT_LIMIT_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharEngineTemperatureUUID,   BLE_RC_ENGINE_TEMPERATURE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharMcuTemperatureUUID,      BLE_RC_MCU_TEMPERATURE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryMinVoltageUUID,   BLE_RC_BATTERY_MIN_VOLTAGE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharAllStatsUUID,            BLE_RC_ALL_STATS_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharAllDriveUUID,            BLE_RC_ALL_DRIVE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharDeviceCapabilitiesUUID,  BLE_RC_DEVICE_CAPABILITIES_CHAR_UUID);

static CONST gattAttrType_t	bleRCProfileService	= { DL_UUID_SIZE, bleRCProfileServUUID };

static uint8 bleThrottleDescription[22]           = "Throttle\0";
static uint8 bleSteeringDescription[22]           = "Steering\0";
static uint8 blePulseWidthDescription[22]         = "Pulse Width\0";
static uint8 bleBatteryVoltageDescription[22]     = "Battery Voltage\0";
static uint8 bleLightsDescription[22]             = "Lights\0";
static uint8 bleThrottleMinDescription[22]        = "Throttle Min\0";
static uint8 bleThrottleMaxDescription[22]        = "Throttle Max\0";
static uint8 bleThrottleCenterDescription[22]     = "Throttle Center\0";
static uint8 bleSteeringMinDescription[22]        = "Steering Min\0";
static uint8 bleSteeringMaxDescription[22]        = "Steering Max\0";
static uint8 bleSteeringCenterDescription[22]     = "Steering Center\0";
static uint8 bleBatteryCurrentDescription[22]     = "Battery Current\0";
static uint8 bleBatteryMaxVoltageDescription[22]  = "Battery Max Voltage\0";
static uint8 bleBatteryCapacityDescription[22]    = "Battery Capacity\0";
static uint8 bleBatteryUsedDescription[22]        = "Battery Used\0";
static uint8 bleDeviceNameDescription[22]         = "Device Name\0";
static uint8 bleDevicePassDescription[22]         = "Device Pass\0";
static uint8 bleBatteryCalibrationDescription[22] = "Calibration Register\0";
static uint8 bleMainLightsNoDescription[22]       = "Main Lights No\0";
static uint8 bleBackLightsNoDescription[22]       = "Back Lights No\0";
static uint8 bleReverseLightsNoDescription[22]    = "Reverse Lights No\0";
static uint8 bleLeftLightsNoDescription[22]       = "Left Lights No\0";
static uint8 bleRightLightsNoDescription[22]      = "Right Lights No\0";
static uint8 bleServoPosDescription[22]           = "Servo Position\0";
static uint8 bleCurrentLimitDescription[22]       = "Current Limit\0";
static uint8 bleEngineTemperatureDescription[22]  = "Engine Temperature\0";
static uint8 bleMcuTemperatureDescription[22]     = "MCU Temperature\0";
static uint8 bleBatteryMinVoltageDescription[22]  = "Battery Min Voltage\0";
static uint8 bleAllStatsDescription[22]           = "Stats\0";
static uint8 bleAllDriveDescription[22]           = "Throttle + Steering\0";
static uint8 bleDeviceCapabilitiesDescription[22] = "Device Capabilities\0";

Stats stats;
Control control;

static uint8 bleThrottleProperties           = GATT_PROP_WRITE_NO_RSP;
static uint8 bleSteeringProperties           = GATT_PROP_WRITE_NO_RSP;
static uint8 blePulseWidthProperties         = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryVoltageProperties     = GATT_PROP_READ;
static uint8 bleLightsProperties             = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleThrottleMinProperties        = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleThrottleMaxProperties        = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleThrottleCenterProperties     = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringMinProperties        = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringMaxProperties        = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringCenterProperties     = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryCurrentProperties     = GATT_PROP_READ;
static uint8 bleBatteryMaxVoltageProperties  = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryMinVoltageProperties  = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryCapacityProperties    = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryUsedProperties        = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleDeviceNameProperties         = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleDevicePassProperties         = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryCalibrationProperties = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleMainLightsNoProperties       = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBackLightsNoProperties       = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleReverseLightsNoProperties    = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleLeftLightsNoProperties       = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleRightLightsNoProperties      = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleServoPosProperties           = GATT_PROP_READ;
static uint8 bleCurrentLimitProperties       = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleEngineTemperatureProperties  = GATT_PROP_READ;
static uint8 bleMcuTemperatureProperties     = GATT_PROP_READ;
static uint8 bleAllStatsProperties           = GATT_PROP_READ;
static uint8 bleAllDriveProperties           = GATT_PROP_WRITE_NO_RSP;
static uint8 bleDeviceCapabilitiesProperties = GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;

static gattAttribute_t bleRCProfileAttributesTable[] =
{
	{ { ATT_BT_UUID_SIZE, primaryServiceUUID }, GATT_PERMIT_READ, 0, (uint8*)&bleRCProfileService },

	DECLARE_ATTR(bleThrottleProperties,           bleRCProfileCharThrottleUUID,            GATT_PERMIT_WRITE,                    control.throttle,            bleThrottleDescription),
	DECLARE_ATTR(bleSteeringProperties,           bleRCProfileCharSteeringUUID,            GATT_PERMIT_WRITE,                    control.steering,            bleSteeringDescription),
	DECLARE_ATTR(blePulseWidthProperties,         bleRCProfileCharPulseWidthUUID,          GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.pulseWidth,         blePulseWidthDescription),
	DECLARE_ATTR(bleBatteryVoltageProperties,     bleRCProfileCharBatteryVoltageUUID,      GATT_PERMIT_READ,                     stats.batteryVoltage,        bleBatteryVoltageDescription),
	DECLARE_ATTR(bleLightsProperties,             bleRCProfileCharLightsUUID,              GATT_PERMIT_WRITE | GATT_PERMIT_READ, control.lights,              bleLightsDescription),
	DECLARE_ATTR(bleThrottleMinProperties,        bleRCProfileCharThrottleMinUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.throttleMin,        bleThrottleMinDescription),
	DECLARE_ATTR(bleThrottleMaxProperties,        bleRCProfileCharThrottleMaxUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.throttleMax,        bleThrottleMaxDescription),
	DECLARE_ATTR(bleThrottleCenterProperties,     bleRCProfileCharThrottleCenterUUID,      GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.throttleCenter,     bleThrottleCenterDescription),
	DECLARE_ATTR(bleSteeringMinProperties,        bleRCProfileCharSteeringMinUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.steeringMin,        bleSteeringMinDescription),
	DECLARE_ATTR(bleSteeringMaxProperties,        bleRCProfileCharSteeringMaxUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.steeringMax,        bleSteeringMaxDescription),
	DECLARE_ATTR(bleSteeringCenterProperties,     bleRCProfileCharSteeringCenterUUID,      GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.steeringCenter,     bleSteeringCenterDescription),
	DECLARE_ATTR(bleBatteryCurrentProperties,     bleRCProfileCharBatteryCurrentUUID,      GATT_PERMIT_READ,                     stats.batteryCurrent,        bleBatteryCurrentDescription),
	DECLARE_ATTR(bleBatteryMaxVoltageProperties,  bleRCProfileCharBatteryMaxVoltageUUID,   GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.batteryMaxVoltage,  bleBatteryMaxVoltageDescription),
	DECLARE_ATTR(bleBatteryCapacityProperties,    bleRCProfileCharBatteryCapacityUUID,     GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.batteryCapacity,    bleBatteryCapacityDescription),
	DECLARE_ATTR(bleBatteryUsedProperties,        bleRCProfileCharBatteryUsedUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, stats.batteryUsed,           bleBatteryUsedDescription),
	DECLARE_ATTR(bleDeviceNameProperties,         bleRCProfileCharDeviceNameUUID,          GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.deviceName,         bleDeviceNameDescription),
	DECLARE_ATTR(bleDevicePassProperties,         bleRCProfileCharDevicePassUUID,          GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.devicePassword,     bleDevicePassDescription),
	DECLARE_ATTR(bleBatteryCalibrationProperties, bleRCProfileCharBatteryCalibrrationUUID, GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.batteryCalibration, bleBatteryCalibrationDescription),
	DECLARE_ATTR(bleMainLightsNoProperties,       bleRCProfileCharMainLightNoUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.mainLightNo,        bleMainLightsNoDescription),
	DECLARE_ATTR(bleBackLightsNoProperties,       bleRCProfileCharBackLightNoUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.backLightNo,        bleBackLightsNoDescription),
	DECLARE_ATTR(bleReverseLightsNoProperties,    bleRCProfileCharReverseLightNoUUID,      GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.reverseLightNo,     bleReverseLightsNoDescription),
	DECLARE_ATTR(bleLeftLightsNoProperties,       bleRCProfileCharLeftLightNoUUID,         GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.leftLightNo,        bleLeftLightsNoDescription),
	DECLARE_ATTR(bleRightLightsNoProperties,      bleRCProfileCharRightLightNoUUID,        GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.rightLightNo,       bleRightLightsNoDescription),
	DECLARE_ATTR(bleServoPosProperties,           bleRCProfileCharServoPosUUID,            GATT_PERMIT_READ,                     stats.servoPosition,         bleServoPosDescription),
	DECLARE_ATTR(bleCurrentLimitProperties,       bleRCProfileCharCurrentLimitUUID,        GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.currentLimit,       bleCurrentLimitDescription),
	DECLARE_ATTR(bleEngineTemperatureProperties,  bleRCProfileCharEngineTemperatureUUID,   GATT_PERMIT_READ,                     stats.engineTemperature,     bleEngineTemperatureDescription),
	DECLARE_ATTR(bleMcuTemperatureProperties,     bleRCProfileCharMcuTemperatureUUID,      GATT_PERMIT_READ,                     stats.mcuTemperature,        bleMcuTemperatureDescription),
	DECLARE_ATTR(bleBatteryMinVoltageProperties,  bleRCProfileCharBatteryMinVoltageUUID,   GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.batteryMinVoltage,  bleBatteryMinVoltageDescription),
	DECLARE_ATTR(bleAllStatsProperties,           bleRCProfileCharAllStatsUUID,            GATT_PERMIT_READ,                     stats,                       bleAllStatsDescription),
	DECLARE_ATTR(bleAllDriveProperties,           bleRCProfileCharAllDriveUUID,            GATT_PERMIT_WRITE,                    control,                     bleAllDriveDescription),
	DECLARE_ATTR(bleDeviceCapabilitiesProperties, bleRCProfileCharDeviceCapabilitiesUUID,  GATT_PERMIT_WRITE | GATT_PERMIT_READ, settings.deviceCapabilities, bleDeviceCapabilitiesDescription)
};

static uint8 RaceCarProfileReadAttributeCallBack(uint16 connHandle, gattAttribute_t* attribute, uint8* value, uint8* length, uint16 offset, uint8 maxLen)
{
	bStatus_t status = SUCCESS;

	if (gattPermitAuthorRead(attribute->permissions))
	{
		return (ATT_ERR_INSUFFICIENT_AUTHOR);
	}

	if (offset > 0)
	{
		return (ATT_ERR_ATTR_NOT_LONG);
	}

	uint16 uuid;

	if (ExtractUuid16(attribute, &uuid) == FAILURE)
	{
		*length = 0;
		
		return ATT_ERR_INVALID_HANDLE;
	}
	
	switch (uuid)
	{
		case BLE_RC_THROTTLE_CHAR_UUID:
		case BLE_RC_STEERING_CHAR_UUID:
		case BLE_RC_PULSE_WIDTH_CHAR_UUID:
		case BLE_RC_BATTERY_VOLTAGE_CHAR_UUID:
		case BLE_RC_LIGHTS_CHAR_UUID:
		case BLE_RC_THROTTLE_MIN_CHAR_UUID:
		case BLE_RC_THROTTLE_MAX_CHAR_UUID:
		case BLE_RC_THROTTLE_CENTER_CHAR_UUID:
		case BLE_RC_STEERING_MIN_CHAR_UUID:
		case BLE_RC_STEERING_MAX_CHAR_UUID:
		case BLE_RC_STEERING_CENTER_CHAR_UUID:
		case BLE_RC_BATTERY_CURRENT_CHAR_UUID:
		case BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID:
		case BLE_RC_BATTERY_MIN_VOLTAGE_CHAR_UUID:
		case BLE_RC_BATTERY_CAPACITY_CHAR_UUID:
		case BLE_RC_BATTERY_USED_CHAR_UUID:
		case BLE_RC_BATTERY_CALIBRATION_CHAR_UUID:
		case BLE_RC_MAIN_LIGHT_NO_CHAR_UUID:
		case BLE_RC_BACK_LIGHT_NO_CHAR_UUID:
		case BLE_RC_REVERSE_LIGHT_NO_CHAR_UUID:
		case BLE_RC_LEFT_LIGHT_NO_CHAR_UUID:
		case BLE_RC_RIGHT_LIGHT_NO_CHAR_UUID:
		case BLE_RC_SERVO_POS_CHAR_UUID:
		case BLE_RC_CURRENT_LIMIT_CHAR_UUID:
		case BLE_RC_ENGINE_TEMPERATURE_CHAR_UUID:
		case BLE_RC_MCU_TEMPERATURE_CHAR_UUID:
		case BLE_RC_DEVICE_CAPABILITIES_CHAR_UUID:
			*length = 2;
			value[0] = attribute->pValue[0];
			value[1] = attribute->pValue[1];
			break;
		case BLE_RC_DEVICE_NAME_CHAR_UUID:
		  	*length = GAP_DEVICE_NAME_LEN;
			for (int i = 0; i < GAP_DEVICE_NAME_LEN; i++)
			{
				value[i] = settings.deviceScanNameAddress[i];
			}
			break;
		case BLE_RC_DEVICE_PASS_CHAR_UUID:
		  	*length = 4;
			for (int i = 0; i < 4; i++)
			{
				value[i] = ((uint8*)&(settings.devicePassCode))[i];
			}
			break;
		case BLE_RC_ALL_STATS_CHAR_UUID:
		  	*length = sizeof(Stats);
				
			if (*length <= maxLen)
			{
				for (int i = 0; i < *length; i++)
				{
					value[i] = ((uint8*)&stats)[i];
				}
			}
			break;						
		default:
			*length = 0;
			status = ATT_ERR_ATTR_NOT_FOUND;
			break;
	}

	return status;
};

static bStatus_t AssignValue(gattAttribute_t* attribute, uint8* value, uint8 length, uint16 offset, uint16* result)
{
	if (offset == 0)
	{
		if (length != 2)
		{
			return ATT_ERR_INVALID_VALUE_SIZE;
		}
	}
	else
	{
		return ATT_ERR_ATTR_NOT_LONG;
	}

	uint8* currentValue = (uint8*)attribute->pValue;
	currentValue[0] = value[0];
	currentValue[1] = value[1];

	uint16 v = (currentValue[1] << 8) | currentValue[0];
	
	*result = v;

	return SUCCESS;
};

static bStatus_t AssignAllDriveValue(gattAttribute_t* attribute, uint8* value, uint8 length, uint16 offset, uint16* result)
{
	if (offset == 0)
	{
		if (length != 4)
		{
			return ATT_ERR_INVALID_VALUE_SIZE;
		}
	}
	else
	{
		return ATT_ERR_ATTR_NOT_LONG;
	}

	uint8* currentValue = (uint8*)attribute->pValue;
	currentValue[0] = value[0];
	currentValue[1] = value[1];
	currentValue[2] = value[2];
	currentValue[3] = value[3];

	uint16 t = (currentValue[1] << 8) | currentValue[0];
	uint16 s = (currentValue[3] << 8) | currentValue[2];
	
	result[0] = t;
	result[1] = s;

	return SUCCESS;
};

static bStatus_t SaveDevicePassword(uint16 connectionHandle, gattAttribute_t* pAttr, uint8* pValue, uint8 len, uint16 offset)
{
	if (offset == 0)
	{
		if (len != 4)
		{
			return ATT_ERR_INVALID_VALUE_SIZE;
		}
	}
	else
	{
		return ATT_ERR_ATTR_NOT_LONG;
	}

	for (int i = 0; i < 4; i++)
	{
		((uint8*)&settings.devicePassCode)[i] = pValue[i];
	}

	if (SUCCESS == osal_snv_write(DEVICE_PASSWORD_FLASH_ADDRESS, sizeof(uint32), &settings.devicePassCode))
	{
	}
				   
	GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32), &settings.devicePassCode);
					
	return SUCCESS;
};

void LoadDeviceName(uint8* scanResponseName)
{
	if (SUCCESS == osal_snv_read(DEVICE_NAME_FLASH_ADDRESS, sizeof(uint8) * GAP_DEVICE_NAME_LEN, scanResponseName))
	{
	}
	else
	{
		osal_memcpy(scanResponseName, defaultName, GAP_DEVICE_NAME_LEN);
	}
};

static bStatus_t SaveDeviceName(uint16 connectionHandle, gattAttribute_t* pAttr, uint8* pValue, uint8 len, uint16 offset)
{
	if (offset == 0)
	{
		if ((len >= GAP_DEVICE_NAME_LEN) || (len <= 0))
		{
			return ATT_ERR_INVALID_VALUE_SIZE;
		}
	}
	else
	{
		return ATT_ERR_ATTR_NOT_LONG;
	}

	for (int i = 0; i < len; i++)
	{
		settings.deviceScanNameAddress[i] = pValue[i];
	}

	for (int i = len; i < GAP_DEVICE_NAME_LEN; i++)
	{
		settings.deviceScanNameAddress[i] = 0x0;
	}

	if (SUCCESS == osal_snv_write(DEVICE_NAME_FLASH_ADDRESS, sizeof(uint8) * GAP_DEVICE_NAME_LEN, settings.deviceScanNameAddress))
	{
	}
				   
	GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, settings.deviceScanNameAddress);

	return SUCCESS;
};

static bStatus_t RaceCarProfileWriteAttributeCallBack(uint16 connHandle, gattAttribute_t* attribute, uint8* value, uint8 len, uint16 offset)
{
	bStatus_t status = SUCCESS;

	if (gattPermitAuthorWrite(attribute->permissions))
	{
		return ATT_ERR_INSUFFICIENT_AUTHOR;
	}

	uint16 uuid;

	if (ExtractUuid16(attribute, &uuid) == FAILURE)
	{
		return ATT_ERR_INVALID_HANDLE;
	}
	
	switch (uuid)
	{
		case BLE_RC_THROTTLE_CHAR_UUID:
			BaseCarStartNoDataTimeOut();
			status = AssignValue(attribute, value, len, offset, &control.throttle);
			CarSetThrottle(control.throttle);
			break;
		case BLE_RC_STEERING_CHAR_UUID:
			BaseCarStartNoDataTimeOut();
			status = AssignValue(attribute, value, len, offset, &control.steering);
			CarSetSteering(control.steering);
			break;
		case BLE_RC_PULSE_WIDTH_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.pulseWidth);
			stats.settingsChanged = true;
			break;
		case BLE_RC_LIGHTS_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &control.lights);
			BaseCarSetLights(control.lights);
			break;
		case BLE_RC_THROTTLE_MIN_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.throttleMin);
			stats.settingsChanged = true;
			break;
		case BLE_RC_THROTTLE_MAX_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.throttleMax);
			stats.settingsChanged = true;
			break;
		case BLE_RC_THROTTLE_CENTER_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.throttleCenter);
			stats.settingsChanged = true;
			break;
		case BLE_RC_STEERING_MIN_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.steeringMin);
			stats.settingsChanged = true;
			break;
		case BLE_RC_STEERING_MAX_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.steeringMax);
			stats.settingsChanged = true;
			break;
		case BLE_RC_STEERING_CENTER_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.steeringCenter);
			stats.settingsChanged = true;
			break;
		case BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.batteryMaxVoltage);
			stats.settingsChanged = true;
			break;
		case BLE_RC_BATTERY_MIN_VOLTAGE_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.batteryMinVoltage);
			stats.settingsChanged = true;
			break;
		case BLE_RC_BATTERY_CAPACITY_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.batteryCapacity);
			stats.settingsChanged = true;
			break;
		case BLE_RC_BATTERY_USED_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &stats.batteryUsed);
			break;
		case BLE_RC_BATTERY_CALIBRATION_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.batteryCalibration);
			stats.settingsChanged = true;
			break;
		case BLE_RC_MAIN_LIGHT_NO_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.mainLightNo);
			stats.settingsChanged = true;
			break;
		case BLE_RC_BACK_LIGHT_NO_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.backLightNo);
			stats.settingsChanged = true;
			break;
		case BLE_RC_REVERSE_LIGHT_NO_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.reverseLightNo);
			stats.settingsChanged = true;
			break;
		case BLE_RC_LEFT_LIGHT_NO_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.leftLightNo);
			stats.settingsChanged = true;
			break;
		case BLE_RC_RIGHT_LIGHT_NO_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.rightLightNo);
			stats.settingsChanged = true;
			break;
		case BLE_RC_CURRENT_LIMIT_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.currentLimit);
			stats.settingsChanged = true;
			break;
		case BLE_RC_DEVICE_CAPABILITIES_CHAR_UUID:
			status = AssignValue(attribute, value, len, offset, &settings.deviceCapabilities);
			stats.settingsChanged = true;
			break;
		case BLE_RC_DEVICE_NAME_CHAR_UUID:
			status = SaveDeviceName(connHandle, attribute, value, len, offset);
			break;
		case BLE_RC_DEVICE_PASS_CHAR_UUID:
			status = SaveDevicePassword(connHandle, attribute, value, len, offset);
			break;				
		case BLE_RC_ALL_DRIVE_CHAR_UUID:
			BaseCarStartNoDataTimeOut();
			AssignAllDriveValue(attribute, value, len, offset, &control.throttle);
			CarSetThrottle(control.throttle);
			CarSetSteering(control.steering);
			break;
		case GATT_CLIENT_CHAR_CFG_UUID:
			status = GATTServApp_ProcessCCCWriteReq(connHandle, attribute, value, len, offset, GATT_CLIENT_CFG_NOTIFY);
			break;
		default:
			status = ATT_ERR_ATTR_NOT_FOUND;
			break;
	}

	return (status);
};

static void RaceCarProfileHandleConnStatusCallBack(uint16 connHandle, uint8 changeType)
{
};

CONST gattServiceCBs_t profileCallBacks =
{
	RaceCarProfileReadAttributeCallBack,
	RaceCarProfileWriteAttributeCallBack,
	NULL
};

bStatus_t RaceCarProfileAddService(uint32 services)
{
	uint8 status = SUCCESS;

	linkDB_Register(RaceCarProfileHandleConnStatusCallBack);

	if (services & BLE_RC_PROFILE_SERVICE)
	{
		status = GATTServApp_RegisterService(bleRCProfileAttributesTable, GATT_NUM_ATTRS(bleRCProfileAttributesTable), &profileCallBacks);
	}

	return (status);
};
