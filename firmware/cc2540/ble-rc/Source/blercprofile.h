#ifndef _BLE_RC_PROFILE_H_
#define _BLE_RC_PROFILE_H_

#define BLE_RC_SERVICE_UUID                  0xACC0
#define BLE_RC_THROTTLE_CHAR_UUID            0xACC1
#define BLE_RC_STEERING_CHAR_UUID            0xACC2
#define BLE_RC_ALL_DRIVE_CHAR_UUID           0xACC3
#define BLE_RC_LIGHTS_CHAR_UUID              0xA101
#define BLE_RC_THROTTLE_MIN_CHAR_UUID        0xB101
#define BLE_RC_THROTTLE_MAX_CHAR_UUID        0xB102
#define BLE_RC_THROTTLE_CENTER_CHAR_UUID     0xB103
#define BLE_RC_STEERING_MIN_CHAR_UUID        0xC101
#define BLE_RC_STEERING_MAX_CHAR_UUID        0xC102
#define BLE_RC_STEERING_CENTER_CHAR_UUID     0xC103
#define BLE_RC_PULSE_WIDTH_CHAR_UUID         0xD101
#define BLE_RC_BATTERY_VOLTAGE_CHAR_UUID     0xE101
#define BLE_RC_BATTERY_CURRENT_CHAR_UUID     0xE102
#define BLE_RC_BATTERY_MAX_VOLTAGE_CHAR_UUID 0xE103
#define BLE_RC_BATTERY_CAPACITY_CHAR_UUID    0xE104
#define BLE_RC_BATTERY_USED_CHAR_UUID        0xE105
#define BLE_RC_BATTERY_CALIBRATION_CHAR_UUID 0xE106
#define BLE_RC_CURRENT_LIMIT_CHAR_UUID       0xE107
#define BLE_RC_BATTERY_MIN_VOLTAGE_CHAR_UUID 0xE108
#define BLE_RC_DEVICE_NAME_CHAR_UUID         0xF101
#define BLE_RC_DEVICE_PASS_CHAR_UUID         0xF102
#define BLE_RC_MAIN_LIGHT_NO_CHAR_UUID       0x1001
#define BLE_RC_BACK_LIGHT_NO_CHAR_UUID       0x1002
#define BLE_RC_REVERSE_LIGHT_NO_CHAR_UUID    0x1003
#define BLE_RC_LEFT_LIGHT_NO_CHAR_UUID       0x1004
#define BLE_RC_RIGHT_LIGHT_NO_CHAR_UUID      0x1005
#define BLE_RC_SERVO_POS_CHAR_UUID           0x2001
#define BLE_RC_ENGINE_TEMPERATURE_CHAR_UUID  0x3001
#define BLE_RC_MCU_TEMPERATURE_CHAR_UUID     0x3002
#define BLE_RC_ALL_STATS_CHAR_UUID           0x4001
#define BLE_RC_DEVICE_CAPABILITIES_CHAR_UUID 0x5001
#define BLE_RC_PROFILE_SERVICE               0x0002

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Stats
{
	uint16 engineTemperature;
	uint16 mcuTemperature;
	uint16 batteryVoltage;
	uint16 batteryCurrent;
	uint16 batteryUsed;
	uint16 servoPosition;
	bool connected;
	uint8 settingsChanged;
	bool adjustingServo;
	uint8 mainTaskId;
} Stats;

typedef struct Control
{
	uint16 throttle;
	uint16 steering;	
	uint16 lights;
} Control;

typedef struct Settings
{
	uint16 pulseWidth;
	uint16 throttleMin;
	uint16 throttleMax;
	uint16 throttleCenter;
	uint16 steeringMin;
	uint16 steeringMax;
	uint16 steeringCenter;
	uint16 mainLightNo;
	uint16 backLightNo;
	uint16 reverseLightNo;
	uint16 leftLightNo;
	uint16 rightLightNo;
	uint16 batteryMaxVoltage;
	uint16 batteryMinVoltage;
	uint16 batteryCapacity;
	uint16 batteryCalibration;
	uint16 currentLimit;
	uint16 deviceCapabilities;
	uint16 deviceName;
	uint16 devicePassword;
	uint32 devicePassCode;
	uint8* deviceScanNameAddress;
} Settings;

extern Stats stats;
extern Settings settings;
extern Control control;

extern bStatus_t RaceCarProfileAddService(uint32 services);
extern void RcInit(uint8 taskId, uint8* deviceName, uint32* passCode);
extern void RcStart(uint8 taskId);
extern void RcStop();
extern void RcUpdateStatus();
extern void TempFinishUpdate();
extern void UpdateDeviceName(uint8* deviceName);
extern bool HasCapability(uint16 capability);

enum DeviceCapability
{
	HasIna226 = 1,
	HasIna220 = 2,
	HasAccelerometer = 4,
	HasLights = 8,
//	IsMiniZ = 16,
//	IsRC = 32,
//	IsSimple = 64,
//	IsBrushless = 128,
	HasTemperature = 256
};

extern void BaseCarAccelerometerChangeCallBack(uint8 parameterId);
extern void BaseCarConnected(uint8 taskId);
extern void BaseCarDisConnected();
extern void BaseCarLoadValues();
extern void BaseCarSaveValues();
extern void BaseCarReadAccelerometerData();
extern void BaseCarUpdateStatus();
extern void BaseCarTempFinishUpdate();
extern void BaseCarFinishAccelerometerUpdate();
extern void BaseCarInit(uint8 taskId, uint8* deviceScanName);
extern void BaseCarSetLights(uint16 value);
extern void BaseCarStartNoDataTimeOut();
extern void BaseCarOnNoCommand();
extern void LoadDeviceName(uint8* scanResponseName);

#ifdef __cplusplus
}
#endif

#endif
