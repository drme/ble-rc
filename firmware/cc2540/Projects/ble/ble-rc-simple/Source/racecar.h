#ifndef _RACE_CAR_H_
#define _RACE_CAR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define RC_START_DEVICE_EVT			0x0001
#define RC_UPDATE_STATUS_EVT		0x0004
  
/**
 * Task Initialization for the BLE Application
 */
extern void RaceCarInit(uint8 taskId);

/**
 * Task Event Processor for the BLE Application
 */
extern uint16 RaceCarProcessEvent(uint8 taskId, uint16 events);

#ifdef __cplusplus
}
#endif

#endif
