#ifndef _HAL_INA226_H_
#define _HAL_INA226_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initializes INA226 with calibartion register value.
 */
extern void HalIna226Init(float maxCurrent, float shuntResistorValue);
/**
 * Disables INA226.
 */
extern void HalIna226DeInit();
/**
 * Returns measured bus voltage in mV.
 */
extern void HalIna226UpdateVoltage(uint16* voltage);
/**
 * Return measured current in mA
 */
extern void HalIna226UpdateCurrent(uint16* current);
/**
 * Starts powering INA226 from P0.1 pin
 */
extern void PowerOnIna226();
/**
 * Stops powering INA226 from P0.1 pin
 */
extern void PowerOffIna226();

#ifdef __cplusplus
}
#endif

#endif
