#ifndef _HAL_INA220_H_
#define _HAL_INA220_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern void HalIna220Init(uint16 calibratrionRegister);
extern void HalIna220DeInit();
extern void HalIna220UpdateVoltage(uint16* voltage);
extern void HalIna220UpdateCurrent(uint16* current);
extern void PowerOnIna220();
extern void PowerOffIna220();

#ifdef __cplusplus
}
#endif

#endif
