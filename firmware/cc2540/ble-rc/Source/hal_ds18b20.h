#ifndef _HAL_DS18B20_H_
#define _HAL_DS18B20_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialize DS18B20
 */
extern void HalDS18B20Init();
/**
 * Starts reading temperature, raises specified event then reading is done.
 */
extern void HalDS18B20StartReading(uint8 taskId);
/**
 * Returns temperature.
 */
extern float HalDS18B20FinishReading(uint16* temperature, uint16* humidity);

#ifdef __cplusplus
}
#endif

#endif
