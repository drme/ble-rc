#ifndef ACCELEROMETERSERVICE_H
#define ACCELEROMETERSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define TI_UUID_SIZE        ATT_UUID_SIZE
#define TI_UUID(uuid)       TI_BASE_UUID_128(uuid)
  
  
  
  
  
  
/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define ACCELEROMETER_DATA              2       // R  uint8 - Profile Attribute value
#define ACCELEROMETER_CONF              3       // RW uint8 - Profile Attribute value
#define ACCELEROMETER_PERI              4       // RW uint8 - Profile Attribute Value

// Service UUID
#define ACCELEROMETER_SERV_UUID         0xAA10  // F0000000-0451-4000-B000-00000000-AA10
#define ACCELEROMETER_DATA_UUID         0xAA11
#define ACCELEROMETER_CONF_UUID         0xAA12
#define ACCELEROMETER_PERI_UUID         0xAA13

// Sensor Profile Services bit fields
#define ACCELEROMETER_SERVICE           0x00000002

// Length of sensor data in bytes
#define ACCELEROMETER_DATA_LEN          3

  // Minimum accelerometer period
#define ACCELEROMETER_MIN_PERIOD        100
#define ACCELEROMETER_TIME_UNIT         10 // resolution 10 ms

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef NULL_OK void (*accelChange_t)( uint8 paramID );

typedef struct
{
  accelChange_t        pfnAccelChange;  // Called when characteristic value changes
} accelCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */


/*
 * Acc_AddService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Accel_AddService( uint32 services );

/*
 * Acc_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Accel_RegisterAppCBs( accelCBs_t *appCallbacks );

/*
 * Acc_SetParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Accel_SetParameter( uint8 param, uint8 len, void *value );

/*
 * Acc_GetParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Accel_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#if !defined(HAL_I2C)

void HalAccInit(void);
bool HalAccRead(uint8* buffer);
bool HalAccTest(void);

#endif



#ifdef __cplusplus
}
#endif

#endif /* ACCELEROMETERSERVICE_H */

