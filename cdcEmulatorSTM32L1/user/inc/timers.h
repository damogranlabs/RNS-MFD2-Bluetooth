
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMERS_H
#define __TIMERS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "stm32l1xx.h"

#define US_DELAY_TIMER TIM6
#define US_DELAY_TIMER_TICK 1 // [us]

#define DIN_PIN_US_TIMER TIM7
#define DIN_PIN_US_TIMER_TICK 1 // [us]


  void initUsDelayTimer(void);
  void delayUs(uint32_t us);
  void startUsDelayTimer(void);
  uint32_t stopUsDelayTimer(void);
  uint32_t restartUsDelayTimer(void);
  void resetUsDelayTimer(void);
  uint32_t getUsDelayTimerValue(void);
  bool isUsDelayTimerStarted(void);

  void initMisoStopwatch(void);
  void startMisoStopwatch(void);
  uint32_t stopMisoStopwatch(void);
  uint32_t restartMisoStopwatch(void);
  void setMisoStopwatchTimerValue(uint32_t us);
  void resetMisoStopwatch(void);
  uint32_t getMisoStopwatchTimerValue(void);
  
#ifdef __cplusplus
}
#endif

#endif
