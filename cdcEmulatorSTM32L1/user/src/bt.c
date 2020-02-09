/**
  ******************************************************************************
  * @file           : btCtrl.c
  * @brief          : Bluetooth and other utility functions API
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_it.h"

/* Private includes ----------------------------------------------------------*/

#include "uart_print.h"
#include "bt.h"
#include "io.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
nextBtAction_t nextBtAction;
pulseData_t pulseData;
bool isPlaying;

void initBtData(void)
{
  pulseData.timeMs = 0;
  pulseData.endTimestamp = 0;

  nextBtAction.action = BT_ACT_IDLE;
  nextBtAction.timestamp = 0;
  nextBtAction.count = 0;
}

void initBluetoothPins(void)
{
  NVIC_ClearPendingIRQ(EXTI4_IRQn); // DIN pin interrupt
  LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);

  //LL_GPIO_ResetOutputPin()
  setPinState(PLAY_PAUSE_GPIO_Port, PLAY_PAUSE_Pin, false);
  setPinState(NEXT_GPIO_Port, NEXT_Pin, false);
  setPinState(PREV_GPIO_Port, PREV_Pin, false);
}

void ctrlBtPwr(bool isEnabled)
{
  setPinState(BT_PWR_GPIO_Port, BT_PWR_Pin, isEnabled);
}

void generatePulse(GPIO_TypeDef *port, uint32_t pin, uint32_t timeMs)
{
  // currenty timeMs disabled - BT module 'debunce' key by counting?!? rise/falls?
  if (pulseData.endTimestamp == 0)
  {
    pulseData.endTimestamp = LL_GetTick() + timeMs;
    pulseData.port = port;
    pulseData.pin = pin;
    
    setPinState(port, pin, true);
  }
  else
  {
    // ignore
    printString("s");
  }
}

void handlePulse(void)
{
  if (pulseData.endTimestamp != 0)
  {
    if (LL_GetTick() >= pulseData.endTimestamp)
    {
      printString("x");
      pulseData.endTimestamp = 0;
      
      setPinState(pulseData.port, pulseData.pin, false);
    }
  }
}

void stopPulse()
{
  // set endTimestamp to 1, and handlePulse will turn off pin asap
  pulseData.endTimestamp = 1;
}

void addBluetoothAction(btActions action)
{
  if(nextBtAction.action == action)
  {
    nextBtAction.count++;
    printString("+");
    return;
  }
  else // new action
  {
    nextBtAction.action = action;
    nextBtAction.count = 1;
    switch(action)
    {
      case BT_ACT_PLAY_PAUSE:
        nextBtAction.timestamp = ACTION_NO_DELAY_MSEC;
        break;

      case BT_ACT_NEXT:
      case BT_ACT_PREV:
        nextBtAction.timestamp = LL_GetTick() + ACTION_RADIO_MUTE_DELAY_MSEC;
        break;

      case BT_ACT_PWR_ON:
      case BT_ACT_PWR_OFF:
        nextBtAction.timestamp = ACTION_NO_DELAY_MSEC;
        break;
      
      default:
        printString("?");
        break;
    }
  }
}

void handleBtActions(void)
{
  if (nextBtAction.action != BT_ACT_IDLE)
  {
    if (LL_GetTick() >= nextBtAction.timestamp)
    {
      switch (nextBtAction.action)
      {
        case BT_ACT_PLAY_PAUSE:
          generatePulse(PLAY_PAUSE_GPIO_Port, PLAY_PAUSE_Pin, SHORT_PULSE_MSEC);
          break;

        case BT_ACT_NEXT:
          generatePulse(NEXT_GPIO_Port, NEXT_Pin, SHORT_PULSE_MSEC);
          break;

        case BT_ACT_PREV:
          generatePulse(PREV_GPIO_Port, PREV_Pin, SHORT_PULSE_MSEC);
          break;

        case BT_ACT_PWR_ON:
          ctrlBtPwr(true);
          break;

        case BT_ACT_PWR_OFF:
          ctrlBtPwr(false);
          break;
        
        default:
          printString("!");
          break;
      }
      nextBtAction.count--;
      printString("-");
      if(nextBtAction.count == 0)
      {
        nextBtAction.action = BT_ACT_IDLE;
      }
      else
      {
        nextBtAction.timestamp = LL_GetTick() + ACTION_MIN_DELAY_MSEC;
      }
    }
  }
}