#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "blercprofile.h"
#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_adc.h"
#include "gatt.h"
#include "hci.h"
#include "osal_snv.h"
#include "car_control.h"
#include "gapgattserver.h"
  
#define SERVAPP_NUM_ATTR_SUPPORTED	52

#define DECLARE_UUID(name, ID)							CONST uint8 name[ATT_BT_UUID_SIZE] = { LO_UINT16(ID), HI_UINT16(ID)};
#define DECLARE_ATTR_DESCRIPTION(description)			{ { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, description }
#define DECLARE_ATTR_VALUE(ID, permissions, value)		{ { ATT_BT_UUID_SIZE, ID }, permissions, 0, (uint8*)&value }
#define DECLARE_ATTR_PROPERTIES(properties)				{ { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &properties }
#define SET_VALUE(x)									if (len == sizeof(uint16)) { x = *((uint16*)value); } else { ret = bleInvalidRange; };

#define DECLARE_ATTR(bleProperties, bleUUID, blePermission, bleValue, bleDescription)	DECLARE_ATTR_PROPERTIES(bleProperties), DECLARE_ATTR_VALUE(bleUUID, blePermission, bleValue), DECLARE_ATTR_DESCRIPTION(bleDescription)

DECLARE_UUID(bleRCProfileServUUID, 					BLE_RC_SERVICE_UUID);
DECLARE_UUID(bleRCProfileCharThrottleUUID,			BLE_RC_THROTTLE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringUUID,			BLE_RC_STEERING_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharPulseWidthUUID,		BLE_RC_PULSE_WIDTH_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryVoltageUUID,	BLE_RC_BATTERY_VOLTAGE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharLightsUUID,			BLE_RC_LIGHTS_CHAR_UUID);

DECLARE_UUID(bleRCProfileCharThrottleMinUUID,		BLE_RC_THROTTLE_MIN_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharThrottleMaxUUID,		BLE_RC_THROTTLE_MAX_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharThrottleCenterUUID,	BLE_RC_THROTTLE_CENTER_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringMinUUID,		BLE_RC_STEERING_MIN_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringMaxUUID,		BLE_RC_STEERING_MAX_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharSteeringCenterUUID,	BLE_RC_STEERING_CENTER_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryCurrentUUID,	BLE_RC_BATTERY_CURRENT_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryMaxVoltageUUID,	BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryCapacityUUID,	BLE_RC_BATTERY_CAPACITY_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharBatteryUsedUUID,		BLE_RC_BATTERY_USED_CHAR_UUID);

DECLARE_UUID(bleRCProfileCharDeviceNameUUID,		BLE_RC_DEVICE_NAME_CHAR_UUID);
DECLARE_UUID(bleRCProfileCharDevicePassUUID,		BLE_RC_DEVICE_PASS_CHAR_UUID);

//static rcProfileCBs_t*		rcProfile_AppCBs	= NULL;
static CONST gattAttrType_t	bleRCProfileService	= { ATT_BT_UUID_SIZE, bleRCProfileServUUID };

static uint8 bleThrottleDescription[22]				= "Throttle\0";
static uint8 bleSteeringDescription[22]				= "Steering\0";
static uint8 blePulseWidthDescription[22]			= "Pulse Width\0";
static uint8 bleBatteryVoltageDescription[22]		= "Battery Voltage\0";
static uint8 bleLightsDescription[22]				= "Lights\0";

static uint8 bleThrottleMinDescription[22]			= "Throttle Min\0";
static uint8 bleThrottleMaxDescription[22]			= "Throttle Max\0";
static uint8 bleThrottleCenterDescription[22]		= "Throttle Center\0";
static uint8 bleSteeringMinDescription[22]			= "Steering Min\0";
static uint8 bleSteeringMaxDescription[22]			= "Steering Max\0";
static uint8 bleSteeringCenterDescription[22]		= "Steering Center\0";
static uint8 bleBatteryCurrentDescription[22]		= "Battery Current\0";
static uint8 bleBatteryMaxVoltageDescription[22]	= "Battery Max Voltage\0";
static uint8 bleBatteryCapacityDescription[22]		= "Battery Capacity\0";
static uint8 bleBatteryUsedDescription[22]			= "Battery Used\0";
static uint8 bleDeviceNameDescription[22]			= "Device Name\0";
static uint8 bleDevicePassDescription[22]			= "Device Name\0";

static uint16 bleThrottle			= 0;
static uint16 bleSteering			= 0;
static uint16 blePulseWidth			= 0;
static uint16 bleBatteryVoltage		= 0;
static uint16 bleLights				= 0;

static uint16 bleThrottleMin		= 100;
static uint16 bleThrottleMax		= 400;
static uint16 bleThrottleCenter		= 375;
static uint16 bleSteeringMin		= 200;
static uint16 bleSteeringMax		= 300;
static uint16 bleSteeringCenter		= 250;
static uint16 bleBatteryCurrent		= 0;
static uint16 bleBatteryMaxVoltage	= 7200;
static uint16 bleBatteryCapacity	= 1500;
static uint16 bleBatteryUsed		= 1;
static uint16 bleDeviceName			= 0;
static uint16 bleDevicePass			= 0;

static uint8 bleThrottleProperties			= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringProperties			= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 blePulseWidthProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryVoltageProperties	= GATT_PROP_READ;
static uint8 bleLightsProperties			= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;

static uint8 bleThrottleMinProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleThrottleMaxProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleThrottleCenterProperties	= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringMinProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringMaxProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleSteeringCenterProperties	= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryCurrentProperties	= GATT_PROP_READ;
static uint8 bleBatteryMaxVoltageProperties	= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryCapacityProperties	= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleBatteryUsedProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleDeviceNameProperties		= GATT_PROP_WRITE_NO_RSP | GATT_PROP_READ;
static uint8 bleDevicePassProperties		= GATT_PROP_WRITE | GATT_PROP_READ;

static uint8 settingsChanged	= 0;

uint8* deviceNameAddress = NULL;
uint32* devicePassCodeAddress = NULL;

/**
 * Profile Attributes - Table
 */
static gattAttribute_t bleRCProfileAttributesTable[SERVAPP_NUM_ATTR_SUPPORTED] =
{
	// Simple Profile Service
	{
		{ ATT_BT_UUID_SIZE, primaryServiceUUID },	/* type */
		GATT_PERMIT_READ,							/* permissions */
		0,											/* handle */
		(uint8*)&bleRCProfileService				/* pValue */
	},

	DECLARE_ATTR_PROPERTIES(bleThrottleProperties),
	DECLARE_ATTR_VALUE(bleRCProfileCharThrottleUUID, GATT_PERMIT_WRITE | GATT_PERMIT_READ, bleThrottle),
	DECLARE_ATTR_DESCRIPTION(bleThrottleDescription),

	DECLARE_ATTR_PROPERTIES(bleSteeringProperties),
	DECLARE_ATTR_VALUE(bleRCProfileCharSteeringUUID, GATT_PERMIT_WRITE | GATT_PERMIT_READ, bleSteering),
	DECLARE_ATTR_DESCRIPTION(bleSteeringDescription),

	DECLARE_ATTR_PROPERTIES(blePulseWidthProperties),
	DECLARE_ATTR_VALUE(bleRCProfileCharPulseWidthUUID, GATT_PERMIT_WRITE | GATT_PERMIT_READ, blePulseWidth),
	DECLARE_ATTR_DESCRIPTION(blePulseWidthDescription),

	DECLARE_ATTR_PROPERTIES(bleBatteryVoltageProperties),
	DECLARE_ATTR_VALUE(bleRCProfileCharBatteryVoltageUUID, GATT_PERMIT_READ, bleBatteryVoltage),
	DECLARE_ATTR_DESCRIPTION(bleBatteryVoltageDescription),

	DECLARE_ATTR_PROPERTIES(bleLightsProperties),
	DECLARE_ATTR_VALUE(bleRCProfileCharLightsUUID, GATT_PERMIT_WRITE | GATT_PERMIT_READ, bleLights),
	DECLARE_ATTR_DESCRIPTION(bleLightsDescription),
	
	DECLARE_ATTR_PROPERTIES(bleThrottleMinProperties),			DECLARE_ATTR_VALUE(bleRCProfileCharThrottleMinUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleThrottleMin),		DECLARE_ATTR_DESCRIPTION(bleThrottleMinDescription),
	DECLARE_ATTR_PROPERTIES(bleThrottleMaxProperties),			DECLARE_ATTR_VALUE(bleRCProfileCharThrottleMaxUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleThrottleMax),		DECLARE_ATTR_DESCRIPTION(bleThrottleMaxDescription),
	DECLARE_ATTR_PROPERTIES(bleThrottleCenterProperties),		DECLARE_ATTR_VALUE(bleRCProfileCharThrottleCenterUUID,		GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleThrottleCenter),		DECLARE_ATTR_DESCRIPTION(bleThrottleCenterDescription),
	DECLARE_ATTR_PROPERTIES(bleSteeringMinProperties),			DECLARE_ATTR_VALUE(bleRCProfileCharSteeringMinUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleSteeringMin),		DECLARE_ATTR_DESCRIPTION(bleSteeringMinDescription),
	DECLARE_ATTR_PROPERTIES(bleSteeringMaxProperties),			DECLARE_ATTR_VALUE(bleRCProfileCharSteeringMaxUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleSteeringMax),		DECLARE_ATTR_DESCRIPTION(bleSteeringMaxDescription),
	DECLARE_ATTR_PROPERTIES(bleSteeringCenterProperties),		DECLARE_ATTR_VALUE(bleRCProfileCharSteeringCenterUUID,		GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleSteeringCenter),		DECLARE_ATTR_DESCRIPTION(bleSteeringCenterDescription),
	DECLARE_ATTR_PROPERTIES(bleBatteryCurrentProperties),		DECLARE_ATTR_VALUE(bleRCProfileCharBatteryCurrentUUID,		GATT_PERMIT_READ,						bleBatteryCurrent),		DECLARE_ATTR_DESCRIPTION(bleBatteryCurrentDescription),
	DECLARE_ATTR_PROPERTIES(bleBatteryMaxVoltageProperties),	DECLARE_ATTR_VALUE(bleRCProfileCharBatteryMaxVoltageUUID,	GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleBatteryMaxVoltage),	DECLARE_ATTR_DESCRIPTION(bleBatteryMaxVoltageDescription),
	DECLARE_ATTR(bleBatteryCapacityProperties,	bleRCProfileCharBatteryCapacityUUID,	GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleBatteryCapacity,	bleBatteryCapacityDescription),
	DECLARE_ATTR(bleBatteryUsedProperties,		bleRCProfileCharBatteryUsedUUID,		GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleBatteryUsed,		bleBatteryUsedDescription),
	DECLARE_ATTR(bleDeviceNameProperties,		bleRCProfileCharDeviceNameUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleDeviceName,		bleDeviceNameDescription),
	DECLARE_ATTR(bleDevicePassProperties,		bleRCProfileCharDevicePassUUID,			GATT_PERMIT_WRITE | GATT_PERMIT_READ,	bleDevicePass,		bleDevicePassDescription)
};

static uint8 rcProfile_ReadAttrCB(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint8* pLen, uint16 offset, uint8 maxLen);
static bStatus_t rcProfile_WriteAttrCB(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint8 len, uint16 offset);
static void rcProfile_HandleConnStatusCB(uint16 connHandle, uint8 changeType);

/**
 * Service Callbacks
 */
CONST gattServiceCBs_t rcProfileCBs =
{
	rcProfile_ReadAttrCB,	// Read callback function pointer
	rcProfile_WriteAttrCB,	// Write callback function pointer
	NULL					// Authorization callback function pointer
};

/**
 * Initializes the Profile service by registering GATT attributes with the GATT server.
 *
 * @param services - services to add. This is a bit map and can contain more than one service.
 * @return Success or Failure
 */
bStatus_t RcProfileAddService(uint32 services)
{
	uint8 status = SUCCESS;

	// Register with Link DB to receive link status change callback
	VOID linkDB_Register(rcProfile_HandleConnStatusCB);

	if (services & BLE_RC_PROFILE_SERVICE)
	{
		// Register GATT attribute list and CBs with GATT Server App
		status = GATTServApp_RegisterService(bleRCProfileAttributesTable, GATT_NUM_ATTRS(bleRCProfileAttributesTable), &rcProfileCBs);
	}

	return (status);
};

/**
 * Registers the application callback function. Only call this function once.
 *
 * @param callbacks - pointer to application callbacks.
 * @return SUCCESS or bleAlreadyInRequestedMode.
 */
bStatus_t rcProfile_RegisterAppCBs(rcProfileCBs_t* appCallbacks)
{
	if (appCallbacks)
	{
		//rcProfile_AppCBs = appCallbacks;

		return (SUCCESS);
	}
	else
	{
		return (bleAlreadyInRequestedMode);
	}
};

void saveValues()
{
	uint16 data[] = { blePulseWidth, bleThrottleMin, bleThrottleMax, bleThrottleCenter, bleSteeringMin, bleSteeringMax, bleSteeringCenter, bleBatteryMaxVoltage, bleBatteryCapacity };

	if (SUCCESS == osal_snv_write(0x90, sizeof(uint16) * 9, data))
	{
	}
	else
	{
		blePulseWidth = 0x1111;
	}
};

void loadValues()
{
	uint16 data[] = { 1,  2, 3, 4, 5, 6, 7, 8, 9 };

	if (SUCCESS == osal_snv_read(0x90, sizeof(uint16) * 9, data))
	{
		blePulseWidth			= data[0];
		bleThrottleMin			= data[1];
		bleThrottleMax			= data[2];
		bleThrottleCenter		= data[3];
		bleSteeringMin			= data[4];
		bleSteeringMax			= data[5];
		bleSteeringCenter		= data[6];
		bleBatteryMaxVoltage	= data[7];
		bleBatteryCapacity		= data[8];
	}
	else
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
	}
};

/**
 * Read an attribute.
 *
 * @param connHandle - connection message was received on.
 * @param pAttr - pointer to attribute.
 * @param pValue - pointer to data to be read.
 * @param pLen - length of data to be read.
 * @param offset - offset of the first octet to be read.
 * @param maxLen - maximum length of data to be read.
 * @return Success or Failure.
 */
static uint8 rcProfile_ReadAttrCB(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint8* pLen, uint16 offset, uint8 maxLen)
{
	bStatus_t status = SUCCESS;

	// If attribute permissions require authorization to read, return error
	if (gattPermitAuthorRead(pAttr->permissions))
	{
		// Insufficient authorization
		return (ATT_ERR_INSUFFICIENT_AUTHOR);
	}

	// Make sure it's not a blob operation (no attributes in the profile are long)
	if (offset > 0)
	{
		return (ATT_ERR_ATTR_NOT_LONG);
	}

	if (pAttr->type.len == ATT_BT_UUID_SIZE)
	{
		// 16-bit UUID
		uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

		switch ( uuid )
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
			case BLE_RC_BATTERY_CAPACITY_CHAR_UUID:
			case BLE_RC_BATTERY_USED_CHAR_UUID:
				*pLen = 2;
				pValue[0] = pAttr->pValue[0];
				pValue[1] = pAttr->pValue[1];
				break;
			case BLE_RC_DEVICE_NAME_CHAR_UUID:
			  	*pLen = GAP_DEVICE_NAME_LEN;
				for (int i = 0; i < GAP_DEVICE_NAME_LEN; i++)
				{
					pValue[i] = deviceNameAddress[i];
				}
				break;
			case BLE_RC_DEVICE_PASS_CHAR_UUID:
			  	*pLen = 4;
				for (int i = 0; i < 4; i++)
				{
					pValue[i] = ((uint8*)devicePassCodeAddress)[i];
				}
				break;				
			default:
				*pLen = 0;
				status = ATT_ERR_ATTR_NOT_FOUND;
				break;
		}
	}
	else
	{
		// 128-bit UUID
		*pLen = 0;
		status = ATT_ERR_INVALID_HANDLE;
	}

	return (status);
};

/**
 * Validate attribute data prior to a write operation.
 *
 * @param connHandle - connection message was received on.
 * @param pAttr - pointer to attribute.
 * @param pValue - pointer to data to be written.
 * @param len - length of data.
 * @param offset - offset of the first octet to be written.
 * @param complete - whether this is the last packet.
 * @param oper - whether to validate and/or write attribute value  .
 * @return  Success or Failure.
 */
static bStatus_t assignValue(gattAttribute_t* pAttr, uint8* pValue, uint8 len, uint16 offset, uint16* result)
{
	if (offset == 0)
	{
		if (len != 2)
		{
			return ATT_ERR_INVALID_VALUE_SIZE;
		}
	}
	else
	{
		return ATT_ERR_ATTR_NOT_LONG;
	}

	uint8* pCurValue = (uint8*)pAttr->pValue;
	pCurValue[0] = pValue[0];
	pCurValue[1] = pValue[1];

	uint16 v = (pCurValue[1] << 8) | pCurValue[0];
	
	*result = v;

	return SUCCESS;
};

static bStatus_t rcProfile_WriteAttrCB(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint8 len, uint16 offset)
{
	bStatus_t status = SUCCESS;

	 // If attribute permissions require authorization to write, return error
	if (gattPermitAuthorWrite(pAttr->permissions))
	{
		// Insufficient authorization
		return ATT_ERR_INSUFFICIENT_AUTHOR;
	}

	if (pAttr->type.len == ATT_BT_UUID_SIZE)
	{
    	// 16-bit UUID
		uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

		switch (uuid)
		{
			case BLE_RC_THROTTLE_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleThrottle);
				CarSetThrottle(bleThrottle);
				break;
			case BLE_RC_STEERING_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleSteering);
				CarSetSteering(bleSteering);
				break;
			case BLE_RC_PULSE_WIDTH_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &blePulseWidth);
				settingsChanged = true;
				break;
			case BLE_RC_LIGHTS_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleLights);
				CarSetLights(bleLights);
				break;
			case BLE_RC_THROTTLE_MIN_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleThrottleMin);
				settingsChanged = true;
				break;
			case BLE_RC_THROTTLE_MAX_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleThrottleMax);
				settingsChanged = true;
				break;
			case BLE_RC_THROTTLE_CENTER_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleThrottleCenter);
				settingsChanged = true;
				break;
			case BLE_RC_STEERING_MIN_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleSteeringMin);
				settingsChanged = true;
				break;
			case BLE_RC_STEERING_MAX_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleSteeringMax);
				settingsChanged = true;
				break;
			case BLE_RC_STEERING_CENTER_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleSteeringCenter);
				settingsChanged = true;
				break;
			case BLE_RC_BATTERY_CURRENT_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleBatteryCurrent);
				break;
			case BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleBatteryMaxVoltage);
				settingsChanged = true;
				break;
			case BLE_RC_BATTERY_CAPACITY_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleBatteryCapacity);
				settingsChanged = true;
				break;
			case BLE_RC_BATTERY_USED_CHAR_UUID:
				status = assignValue(pAttr, pValue, len, offset, &bleBatteryUsed);
				break;
			case BLE_RC_DEVICE_NAME_CHAR_UUID:
				{
					if (offset == 0)
					{
						if ((len >= GAP_DEVICE_NAME_LEN) || (len <= 0))
						{
							status = ATT_ERR_INVALID_VALUE_SIZE;
						}
					}
					else
					{
						status = ATT_ERR_ATTR_NOT_LONG;
					}

//				uint8* pCurValue = (uint8*)pAttr->pValue;

					for (int i = 0; i < len; i++)
					{
						deviceNameAddress[i] = pValue[i];
					}
					for (int i = len; i < GAP_DEVICE_NAME_LEN; i++)
					{
						deviceNameAddress[i] = 0x0;
					}

					if (SUCCESS == osal_snv_write(0x91, sizeof(uint8) * GAP_DEVICE_NAME_LEN, deviceNameAddress))
					{
					}
					else
					{
						blePulseWidth = 0x1111;
					}				
				   
				   	GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, deviceNameAddress);
					
					status = SUCCESS;
				}
				break;
			case BLE_RC_DEVICE_PASS_CHAR_UUID:
				{
					if (offset == 0)
					{
						if (len != 4)
						{
							status = ATT_ERR_INVALID_VALUE_SIZE;
						}
					}
					else
					{
						status = ATT_ERR_ATTR_NOT_LONG;
					}

//				uint8* pCurValue = (uint8*)pAttr->pValue;

					for (int i = 0; i < 4; i++)
					{
						((uint8*)devicePassCodeAddress)[i] = pValue[i];
					}

					if (SUCCESS == osal_snv_write(0x92, sizeof(uint32), devicePassCodeAddress))
					{
					}
					else
					{
						blePulseWidth = 0x1111;
					}				
				   
					GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE,	sizeof(uint32),	devicePassCodeAddress);
					
					status = SUCCESS;
				}
				break;				
			case GATT_CLIENT_CHAR_CFG_UUID:
				status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
				break;
			default:
				status = ATT_ERR_ATTR_NOT_FOUND;
				break;
		}
	}
	else
	{
		// 128-bit UUID
		status = ATT_ERR_INVALID_HANDLE;
	}

	return (status);
};

/**
 * Profile link status change handler function.
 *
 * @param connHandle - connection handle
 * @param changeType - type of change
 */
static void rcProfile_HandleConnStatusCB(uint16 connHandle, uint8 changeType)
{
};

void RcInit(uint8* deviceName, uint32* passCode)
{
	deviceNameAddress = deviceName;
	devicePassCodeAddress = passCode;
  
	loadValues();

	CarInit();
};

void RcStart(uint8 taskId)
{
	CarConnected(taskId);

	settingsChanged = 0;
};

void RcStop()
{
	CarDisConnected();

	if (settingsChanged > 0)
	{
		saveValues();
	}
};

void RcUpdateStatus()
{
	HalAdcSetReference(HAL_ADC_REF_125V);

	uint16 adc = HalAdcRead(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_10);
  
	bleBatteryVoltage = (uint16)((float)adc * 10.0 * 1.25f * 3.0f / 511.0f);
	  
	CarUpdateStatus();
};
