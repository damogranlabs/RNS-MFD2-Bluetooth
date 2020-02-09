/**
  ******************************************************************************
  * @file           : timers.c
  * @brief          : Delay and MFD IO timers API
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "stm32l1xx.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_tim.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"

#include "io.h"
#include "timers.h"

#include "uart_print.h" // TODO

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
void _waitUntilUpdate(TIM_TypeDef *TIMx);
uint32_t _tickToUs(uint32_t tick, uint32_t timerTick);
uint32_t _usToTick(uint32_t us, uint32_t timerTick);

LL_RCC_ClocksTypeDef clocks;

void initUsDelayTimer(void)
{
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  LL_RCC_GetSystemClocksFreq(&clocks);
  uint32_t prescaller = US_DELAY_TIMER_TICK * clocks.PCLK1_Frequency / 1e6 - 1;
  assert_param(prescaller < 65535);
  LL_TIM_SetPrescaler(US_DELAY_TIMER, prescaller);

  LL_TIM_SetAutoReload(US_DELAY_TIMER, 65535);
  LL_TIM_ClearFlag_UPDATE(US_DELAY_TIMER);
  LL_TIM_EnableIT_UPDATE(US_DELAY_TIMER);
  LL_TIM_GenerateEvent_UPDATE(US_DELAY_TIMER); // prescaller values is updated at the next update event
  _waitUntilUpdate(US_DELAY_TIMER);
  LL_TIM_ClearFlag_UPDATE(US_DELAY_TIMER);

  NVIC_DisableIRQ(TIM6_IRQn);
}

void delayUs(uint32_t us)
{
  if (LL_TIM_IsEnabledCounter(US_DELAY_TIMER))
  {
    Error_Handler();
  }

  uint32_t tickValue = _usToTick(us, US_DELAY_TIMER_TICK) - 1; // we are polling timer counter value and stop as soon as we reach this value.
                                                               // -1 is here because of each increment is counted, including 0 -> 1 tick.
  assert_param(tickValue > 0);
  assert_param(tickValue < 65535);

  LL_TIM_SetCounter(US_DELAY_TIMER, 0);
  LL_TIM_EnableCounter(US_DELAY_TIMER);
  while (LL_TIM_GetCounter(US_DELAY_TIMER) != tickValue)
  {
  }
  LL_TIM_DisableCounter(US_DELAY_TIMER);
}

void startUsDelayTimer(void)
{
  LL_TIM_SetCounter(US_DELAY_TIMER, 0);
  LL_TIM_EnableCounter(US_DELAY_TIMER);
  while (LL_TIM_IsEnabledCounter(US_DELAY_TIMER) == 0)
  {
  };
}

uint32_t restartUsDelayTimer(void)
{
  uint32_t value = stopUsDelayTimer();
  startUsDelayTimer();
  return value;
}

void resetUsDelayTimer(void)
{
  LL_TIM_SetCounter(US_DELAY_TIMER, 0);
}

uint32_t stopUsDelayTimer(void)
{
  LL_TIM_DisableCounter(US_DELAY_TIMER);

  return getUsDelayTimerValue();
}

uint32_t getUsDelayTimerValue(void)
{
  uint32_t tick = LL_TIM_GetCounter(US_DELAY_TIMER); 
  return _tickToUs(tick, US_DELAY_TIMER_TICK);
}

bool isUsDelayTimerStarted(void)
{
  if (LL_TIM_IsEnabledCounter(US_DELAY_TIMER))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**************************************************************************************************/
void initMisoStopwatch(void)
{
  // Timer used for tracking GPIO pin (radio data out, cdcEmulator data in) pulse width.
  // It has enabled update interrupt to check for data transmission error.
  NVIC_ClearPendingIRQ(TIM7_IRQn);

  LL_RCC_GetSystemClocksFreq(&clocks);
  uint32_t prescaller = DIN_PIN_US_TIMER_TICK * clocks.PCLK1_Frequency / 1e6 - 1;
  assert_param(prescaller < 65535);

  LL_TIM_SetPrescaler(DIN_PIN_US_TIMER, prescaller);
  LL_TIM_SetAutoReload(DIN_PIN_US_TIMER, 65535);

  LL_TIM_ClearFlag_UPDATE(DIN_PIN_US_TIMER);
  LL_TIM_EnableIT_UPDATE(DIN_PIN_US_TIMER);
  LL_TIM_GenerateEvent_UPDATE(DIN_PIN_US_TIMER); // Values are updated at the next update event
  _waitUntilUpdate(DIN_PIN_US_TIMER);
  LL_TIM_ClearFlag_UPDATE(DIN_PIN_US_TIMER);

  NVIC_DisableIRQ(TIM7_IRQn);
}

void startMisoStopwatch(void)
{
  LL_TIM_DisableCounter(DIN_PIN_US_TIMER);
  LL_TIM_SetCounter(DIN_PIN_US_TIMER, 0);
  LL_TIM_EnableCounter(DIN_PIN_US_TIMER);

  while (LL_TIM_IsEnabledCounter(DIN_PIN_US_TIMER) == 0)
  {
  };
}

uint32_t stopMisoStopwatch(void)
{
  LL_TIM_DisableCounter(DIN_PIN_US_TIMER);
  uint32_t tick = LL_TIM_GetCounter(DIN_PIN_US_TIMER);
  return _tickToUs(tick, DIN_PIN_US_TIMER_TICK);
}

void setMisoStopwatchTimerValue(uint32_t us)
{
  uint32_t tick = _usToTick(us, DIN_PIN_US_TIMER_TICK);
  LL_TIM_SetCounter(DIN_PIN_US_TIMER, tick);
}

uint32_t restartMisoStopwatch(void)
{
  uint32_t us = stopMisoStopwatch();
  startMisoStopwatch();
  return us;
}

void resetMisoStopwatch(void)
{
  LL_TIM_SetCounter(DIN_PIN_US_TIMER, 0);
}

uint32_t getMisoStopwatchTimerValue(void)
{
  uint32_t tick = LL_TIM_GetCounter(DIN_PIN_US_TIMER);
  return _tickToUs(tick, DIN_PIN_US_TIMER_TICK);
}

/**************************************************************************************************/
uint32_t _tickToUs(uint32_t tick, uint32_t timerTick)
{
  return (uint32_t)tick * timerTick;
}

uint32_t _usToTick(uint32_t us, uint32_t timerTick)
{
  uint32_t tick = us / timerTick;
  assert_param(tick < 65535);

  return tick;
}

void _waitUntilUpdate(TIM_TypeDef *TIMx)
{
  while ((TIMx->EGR & TIM_EGR_UG) == SET)
  {
  }
}