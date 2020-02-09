
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IO_H
#define __IO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#define ONBOARD_GREEN_LED_PORT LD3_GPIO_Port
#define ONBOARD_GREEN_LED_PIN LD3_Pin

#define ONBOARD_BLUE_LED_PORT LD4_GPIO_Port
#define ONBOARD_BLUE_LED_PIN LD4_Pin

#define EXTERNAL_RED_LED_PORT LD5_GPIO_Port
#define EXTERNAL_RED_LED_PIN LD5_Pin

  void setPinState(GPIO_TypeDef *port, uint32_t pin, bool state);

  void ctrlGreenLed(bool state);
  void toggleGreenLed(void);
  
  void ctrlBlueLed(bool state);
  void toggleBlueLed(void);
  
  void ctrlErrorLed(bool state);
  void toggleErrorLed(void);

#ifdef __cplusplus
}
#endif

#endif
