/**
  ******************************************************************************
  * @file           : uart_io.c
  * @brief          : Interface between STM32 and PC over UART
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "uart_print.h"

#include "uart_io.h"
#include "io.h"

#include "mfd_io.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern volatile rb_att_t uartRxBuff;

extern rb_att_t mosiBuff;

/* Private function prototypes -----------------------------------------------*/
void handleLedActions(uint8_t *data);
void handleUartMfdActions(uint8_t *data);

void uartRxDataHandler(void)
{
	// handle UART received data
	uint8_t numberOfBytes = 0;
	uint8_t data[UCMD_RX_DATA_SIZE];

	numberOfBytes = ring_buffer_size(&uartRxBuff);
	if (numberOfBytes < UCMD_RX_DATA_SIZE)
	{
		return;
	}

	if (ring_buffer_get(&uartRxBuff, data, UCMD_RX_DATA_SIZE) == RB_OK)
	{
		printString("CMD: ");
		writeData(data, numberOfBytes);

		switch (data[0])
		{
		case UCMD_CTRL_LED_TOGGLE:
			handleLedActions(data + 1);
			break;

		case UCMD_MFD_ACTION:
			handleUartMfdActions(data + 1);
			break;

		case UCMD_TEST:
			ring_buffer_put(&mosiBuff, &data[1], 1);
			//if (data[1] == UCMD_TEST_SEND_SPI_BYTE)
			//{
			//	//sendCommand(data[2]);
			//}
			break;

		default:
			// TODO: report error
			printStringLn("!!! Invalid received command");
			ring_buffer_flush(&uartRxBuff);
			break;
		}
	}
	else
	{
		// TODO: report error
		ctrlErrorLed(true);
	}
}

void handleLedActions(uint8_t *data)
{
	switch (data[0])
	{
	case UCMD_CTRL_LED_TOGGLE_RED:
		toggleErrorLed();
		break;

	case UCMD_CTRL_LED_TOGGLE_GREEN:
		toggleGreenLed();
		break;

	case UCMD_CTRL_LED_TOGGLE_BLUE:
		toggleBlueLed();
		break;

	default:
		// TODO: report error
		printString("Invalid received LED command: ");
		printNumberLn(data[0], DEC);
		break;
	}
}

void handleUartMfdActions(uint8_t *data)
{
	switch (data[0])
	{
	case UCMD_MFD_CTRL_ENABLE:
		enableMosiDataHandler(true);
		enableMisoDataHandler(true);
		break;

	case UCMD_MFD_CTRL_DISABLE:
		enableMosiDataHandler(false);
		enableMisoDataHandler(false);
		break;

	case UCMD_MFD_CTRL_MOSI_DATA:
		if (data[1] == UCMD_MFD_CTRL_MOSI_DATA_START)
		{
			enableMosiDataHandler(true);
		}
		else if (data[1] == UCMD_MFD_CTRL_MOSI_DATA_STOP)
		{
			enableMosiDataHandler(false);
		}
		else
		{
			printString("Invalid received MFD MOSI CTRL command: ");
			printNumberLn(data[1], DEC);
		}
		break;

	case UCMD_MFD_CTRL_MISO_DATA:
		if (data[1] == UCMD_MFD_CTRL_MISO_DATA_START)
		{
			enableMisoDataHandler(true);
		}
		else if (data[1] == UCMD_MFD_CTRL_MISO_DATA_STOP)
		{
			enableMisoDataHandler(false);
		}
		else
		{
			printString("Invalid received MFD MISO CTRL command: ");
			printNumberLn(data[1], DEC);
		}
		break;

	case UCMD_MFD_SET_TRACK:
		// TODO
		toggleErrorLed();
		break;

	default:
		// TODO: report error
		printString("Invalid received MFD command: ");
		printNumberLn(data[0], DEC);
		break;
	}
}
