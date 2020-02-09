/*
MUTE/PLAY button does not have effect. It is there merely to 
PLAY/CALL ANSWER is triggered by FFW
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BT_H
#define __BT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#define SHORT_PULSE_MSEC 60
#define LONG_PULSE_MSEC 500
#define INDEFINITE_PULSE_MSEC 0xFFFFFFFF

#define ACTION_NO_DELAY_MSEC 0
#define ACTION_MIN_DELAY_MSEC 200              // min delay between two bluetooth pulses (simulate user PB)
#define ACTION_RADIO_MUTE_DELAY_MSEC 0         // delay that RNS MFD2 mute audio output.
#define ACTION_RADIO_MUTE_DELAY_LONG_MSEC 1900 // if rotary encoder is used, MFD has high un-mute delay!

  typedef enum
  {
    BT_ACT_IDLE = 0,
    BT_ACT_PLAY_PAUSE,
    BT_ACT_NEXT,
    BT_ACT_PREV,
    BT_ACT_PWR_ON,
    BT_ACT_PWR_OFF
  } btActions;

  typedef struct pulseData_t
  {
    GPIO_TypeDef *port;
    uint32_t pin;
    uint32_t timeMs;
    uint32_t endTimestamp;
  } pulseData_t;

  typedef struct nextBtAction_t
  {
    btActions action;
    uint32_t timestamp;
    uint8_t count;
  } nextBtAction_t;

  void initBluetoothPins(void);
  void ctrlBtPwr(bool isEnabled);

  void initBtData(void);

  void addBluetoothAction(btActions action);
  void handleBtActions(void);

  void generatePulse(GPIO_TypeDef *port, uint32_t pin, uint32_t timeMs);
  void handlePulse(void);
  void stopPulse();

#ifdef __cplusplus
}
#endif

#endif
