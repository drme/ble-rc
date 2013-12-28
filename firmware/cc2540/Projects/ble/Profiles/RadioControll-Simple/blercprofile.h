#ifndef _BLE_RC_PROFILE_H_
#define _BLE_RC_PROFILE_H_

#ifdef __cplusplus

extern "C"
{
#endif

#define BLE_RC_SERVICE_UUID						0xACC0

#define BLE_RC_THROTTLE_CHAR_UUID				0xACC1
#define BLE_RC_STEERING_CHAR_UUID				0xACC2

#define BLE_RC_LIGHTS_CHAR_UUID					0xA101

#define BLE_RC_THROTTLE_MIN_CHAR_UUID			0xB101
#define BLE_RC_THROTTLE_MAX_CHAR_UUID			0xB102
#define BLE_RC_THROTTLE_CENTER_CHAR_UUID		0xB103

#define BLE_RC_STEERING_MIN_CHAR_UUID			0xC101
#define BLE_RC_STEERING_MAX_CHAR_UUID			0xC102
#define BLE_RC_STEERING_CENTER_CHAR_UUID		0xC103

#define BLE_RC_PULSE_WIDTH_CHAR_UUID			0xD101

#define BLE_RC_BATTERY_VOLTAGE_CHAR_UUID		0xE101
#define BLE_RC_BATTERY_CURRENT_CHAR_UUID		0xE102
#define BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID	0xE103
#define BLE_RC_BATTERY_CAPACITY_CHAR_UUID		0xE104
#define BLE_RC_BATTERY_USED_CHAR_UUID			0xE105
  
#define BLE_RC_DEVICE_NAME_CHAR_UUID			0xF101
#define BLE_RC_DEVICE_PASS_CHAR_UUID			0xF102

#define BLE_RC_PROFILE_SERVICE				0x00000002

// Callback when a characteristic value has changed
typedef NULL_OK void(*rcProfileChange_t)(uint8 paramID);

typedef struct
{
	rcProfileChange_t	pfnRCProfileChange;	// Called when characteristic value changes
} rcProfileCBs_t;

/**
 * Initializes the GATT Profile service by registering GATT attributes with the GATT server.
 *
 * @param services - services to add. This is a bit map and can contain more than one service.
 */
extern bStatus_t RcProfileAddService(uint32 services);

extern void RcInit(uint8* deviceName, uint32* passCode);
extern void RcStart(uint8 taskId);
extern void RcStop();
extern void RcUpdateStatus();

#ifdef __cplusplus
}
#endif

#endif
