#ifndef _RACE_CAR_H_
#define _RACE_CAR_H_

#ifdef __cplusplus
extern "C"
{
#endif
  
extern void RaceCarInit(uint8 taskId);
extern uint16 RaceCarProcessEvent(uint8 taskId, uint16 events);

#ifdef __cplusplus
}
#endif

#endif
