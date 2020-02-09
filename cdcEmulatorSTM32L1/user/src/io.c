/**
  ******************************************************************************
  * @file           : io.c
  * @brief          : LED and other utility functions API
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_it.h"

/* Private includes ----------------------------------------------------------*/

#include "uart_print.h"
#include "io.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

void setPinState(GPIO_TypeDef *port, uint32_t pin, bool state)
{
  if (state == true)
  {
    //LL_GPIO_SetOutputPin(port, pin);
    WRITE_REG(port->BSRR, pin);
  }
  else
  {
    //LL_GPIO_ResetOutputPin(port, pin);
    WRITE_REG(port->BSRR, (pin << 16));
  }
}

void ctrlGreenLed(bool state)
{
  setPinState(ONBOARD_GREEN_LED_PORT, ONBOARD_GREEN_LED_PIN, state);
}

void ctrlBlueLed(bool state)
{
  setPinState(ONBOARD_BLUE_LED_PORT, ONBOARD_BLUE_LED_PIN, state);
}

void ctrlErrorLed(bool state)
{
  setPinState(EXTERNAL_RED_LED_PORT, EXTERNAL_RED_LED_PIN, state);
}

void toggleGreenLed(void)
{
  LL_GPIO_TogglePin(ONBOARD_GREEN_LED_PORT, ONBOARD_GREEN_LED_PIN);
}

void toggleBlueLed(void)
{
  LL_GPIO_TogglePin(ONBOARD_BLUE_LED_PORT, ONBOARD_BLUE_LED_PIN);
}

void toggleErrorLed(void)
{
  LL_GPIO_TogglePin(EXTERNAL_RED_LED_PORT, EXTERNAL_RED_LED_PIN);
}
