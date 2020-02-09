/**
  ******************************************************************************
  * @file           : mfd_io.c
  * @brief          : Interface between STM32 and RNS MFD over SPI
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "stm32l1xx_ll_spi.h"
#include "stm32l1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "spi.h"

#include "mfd_io.h"
#include "uart_print.h"
#include "io.h"
#include "bt.h"
#include "timers.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void _spiSendData(uint8_t *data, uint8_t size);

void _handleMisoAction(uint8_t action);
void _IDLE_stateHandler(uint8_t currentPinState);
void _SOF_HIGH_stateHandler(uint8_t currentPinState);
void _SOF_LOW_stateHandler(uint8_t currentPinState);
void _DATA_stateHandler(uint8_t currentPinState);

void _reportMisoDataError(char *data);

uint8_t _getPacketByteData(uint8_t byteNum);
uint8_t _getCdNumber(void);
uint8_t _getTrackNumber(void);
uint8_t _getTimeMsbNumber(void);
uint8_t _getTimeLsbNumber(void);
uint8_t _getFrame1Number(void);

void _addBitToData(bool bit);
uint8_t _invertByte(uint8_t data);
/* Private user code ---------------------------------------------------------*/

/**************************************************************************************************/
// MOSI handlers
/**************************************************************************************************/
void initSpiDriver(void)
{
	NVIC_ClearPendingIRQ(SPI1_IRQn);

	MX_SPI1_Init();
	LL_SPI_Enable(SPI_DRIVER);
}

void initMosiBuffer(void)
{
	if (ring_buffer_init(&mosiBuff, MOSI_DATA_BUFFER_SIZE) != RB_OK)
	{
    Error_Handler();
	}
}

void initMosiData(void)
{
	mosiData.mode = CDC_MODE_IDLE;
	mosiData.cd = CDC_DISPLAY_ENABLED_CD;
	mosiData.track = CDC_DISPLAY_TRACK_NUMBER;
	mosiData.time = (CDC_DISPLAY_TIME_MIN << 8) | (CDC_DISPLAY_TIME_SEC);
	
	mosiData.isSendingDeckInfo = true;  //start sending DECK info at the very beginning
	mosiData.cdIndexCount = 0;
	mosiData.state = CDC_STATE_SENDING_CD_INFO;

	ring_buffer_flush(&mosiBuff);

	mosiLowLevelData.isEnabled = false;
	mosiLowLevelData.isSendingPacket = false;
	mosiLowLevelData.currentByteNum = 0;
	mosiLowLevelData.nextByteTimestamp = 0;
}

void mosiDataHandler(void)
{
	uint8_t data = 0;

	if (mosiLowLevelData.isEnabled)
	{
		uint32_t currentTime = getUsDelayTimerValue();

		if (mosiLowLevelData.isSendingPacket)
		{
			if (currentTime >= mosiLowLevelData.nextByteTimestamp) // must be >=, so data is sent if nextByteTimestamp is 0
			{
				data = _getPacketByteData(mosiLowLevelData.currentByteNum);
				_spiSendData(&data, 1);

				mosiLowLevelData.currentByteNum++;
				// check if this is the end of a packet?
				if (mosiLowLevelData.currentByteNum == DOUT_PACKET_BYTES_NUMBER)
				{ // end of a packet
					mosiLowLevelData.currentByteNum = 0;
					mosiLowLevelData.isSendingPacket = false;
					
					// if deck info is sending, increment cd index and/or stop sending deck info
					if (mosiData.isSendingDeckInfo)
					{
						if (mosiData.cdIndexCount == 5){
							mosiData.cdIndexCount = 0;
							mosiData.isSendingDeckInfo = false;
							mosiData.state = CDC_STATE_INITIALIZED;
						}
						else{
							mosiData.cdIndexCount++;
						}
					}
				}
				else
				{ // more bytes needs to be send in this packet
					mosiLowLevelData.nextByteTimestamp = currentTime + DOUT_PACKET_BYTES_DELAY_US;
				}
			}
		}
		else
		{
			// currently not sending packet. Check if we should?
			if (currentTime > DOUT_PACKET_DELAY_US)
			{
				mosiLowLevelData.isSendingPacket = true;
				mosiLowLevelData.nextByteTimestamp = 0;
				mosiLowLevelData.currentByteNum = 0;

				// only change mode if something is in buffer, otherwise left unchanged
				if (ring_buffer_size(&mosiBuff) > 0)
				{
					ring_buffer_get(&mosiBuff, &data, 1);
					mosiData.mode = (cdcMode_t) data;
				}
				
				resetUsDelayTimer();
				// return immediately and send first byte of a packet on this function next entry
			}
		}
	}
}

void enableMosiDataHandler(bool state)
{
	if (state)
	{
		initMosiData();

		mosiLowLevelData.isEnabled = true;
		mosiLowLevelData.isSendingPacket = false;
		mosiLowLevelData.currentByteNum = 0;

		startUsDelayTimer();
	}
	else
	{
		mosiLowLevelData.isEnabled = false;
		stopUsDelayTimer();
	}
}

uint8_t _getPacketByteData(uint8_t byteNum)
{
	uint8_t dataOut = 0xFF;

	switch(byteNum)
	{
		case DOUT_BYTE_MODE_INDEX:
			dataOut = mosiData.mode;	
			break;

		case DOUT_BYTE_CD_INDEX:
			dataOut = _getCdNumber();
			break;

		case DOUT_BYTE_TRACK_INDEX:
			dataOut = _getTrackNumber();
			break;

		case DOUT_BYTE_TIME_MSB_INDEX:
			dataOut = _getTimeMsbNumber();
			break;

		case DOUT_BYTE_TIME_LSB_INDEX:
			dataOut = _getTimeLsbNumber();
			break;

		case DOUT_BYTE_FRAME_1_INDEX:
			dataOut = _getFrame1Number();
			break;

		case DOUT_BYTE_FRAME_2_INDEX:
			dataOut = mosiData.state;
			break;

		case DOUT_BYTE_FRAME_3_INDEX:
			// byte 0 and 7 (last) are always related
			dataOut = mosiData.mode | 0x0C;
			break;

		default:
			break;
	}

	return dataOut;
}

uint8_t _getCdNumber(void)
{
	uint8_t dataOut = 0xFF - CDC_DISPLAY_ENABLED_CD;

	if (mosiData.isSendingDeckInfo)
	{
		dataOut = 0x6F;
		if(mosiData.cdIndexCount == (CDC_DISPLAY_ENABLED_CD-1))
		{
			dataOut &= 0xBF; // 6 bit = 0 to enable chosen CD: ~(1 << 6)
		}
		dataOut -= (mosiData.cdIndexCount + 1);
	}
	
	return dataOut;
}

uint8_t _getTrackNumber(void)
{
	uint8_t dataOut = 0xFF - CDC_DISPLAY_TRACK_NUMBER;

	if (mosiData.isSendingDeckInfo)
	{
		if (mosiData.cdIndexCount == (CDC_DISPLAY_ENABLED_CD - 1))
		{
			dataOut = 0xCC;	
		}
		else
		{
			dataOut = 0x00;
		}
	}

	return dataOut;
}

uint8_t _getTimeMsbNumber(void)
{
	uint8_t dataOut = 0xFF - CDC_DISPLAY_TIME_MIN;

	if (mosiData.isSendingDeckInfo)
	{
		if (mosiData.cdIndexCount == (CDC_DISPLAY_ENABLED_CD - 1))
		{
			dataOut = 0xCC;	
		}
		else
		{
			dataOut = 0xFF;	
		}
	}
	
	return dataOut;
}

uint8_t _getTimeLsbNumber(void)
{
	uint8_t dataOut = 0xFF - CDC_DISPLAY_TIME_SEC;

	if (mosiData.isSendingDeckInfo)
	{
		if (mosiData.cdIndexCount == (CDC_DISPLAY_ENABLED_CD - 1))
		{
			dataOut = 0x86;	
		}
		else
		{
			dataOut = 0xFF;	
		}
	}
	
	return dataOut;
}


uint8_t _getFrame1Number(void)
{
	uint8_t dataOut = 0xFF;

	if (mosiData.isSendingDeckInfo && (mosiData.cdIndexCount == (CDC_DISPLAY_ENABLED_CD-1)))
	{
		dataOut = 0xB7;
	}
	
	return dataOut;
}

void _spiSendData(uint8_t *data, uint8_t size)
{
	uint8_t currentByte;

	for (currentByte = 0; currentByte < size; currentByte++)
	{
		while (!LL_SPI_IsActiveFlag_TXE(SPI_DRIVER))
		{
		}

		LL_SPI_TransmitData8(SPI_DRIVER, *(data + currentByte));

		while (!LL_SPI_IsActiveFlag_TXE(SPI_DRIVER))
		{
		}
	}
}

/**************************************************************************************************/
// MISO handlers
/**************************************************************************************************/
void initMisoData(void)
{
	misoData.isEnabled = false;
	misoData.state = DIN_IDLE;
	misoData.data = 0;
	misoData.capTime = 0;
	misoData.lastPinState = LL_GPIO_IsInputPinSet(DIN_GPIO_Port, DIN_Pin);

	resetMisoStopwatch();

	misoPinChange = false;
}

void misoDataHandler(void)
{
	uint8_t data[DIN_PACKET_BYTES_NUMBER];
	uint8_t packetDataToAdd[1];

	if (misoData.isEnabled)
	{
		uint32_t currentTime = getMisoStopwatchTimerValue();
		if (misoData.state == DIN_DATA_LOW)
		{
			if (currentTime > DIN_EOF_US)
			{
				stopMisoStopwatch();

				data[0] = (uint8_t)((misoData.data) & 0xFFUL);
				data[1] = (uint8_t)((misoData.data >> 8) & 0xFFUL);
				data[2] = (uint8_t)((misoData.data >> 16) & 0xFFUL);
				data[3] = (uint8_t)((misoData.data >> 24) & 0xFFUL);

				initMisoData();
				misoData.isEnabled = true;
				
				if ((data[0] == DIN_BYTE_0_VALUE) && (data[1] == DIN_BYTE_1_VALUE))
				{
					if (data[2] == _invertByte(data[3]))
					{
						packetDataToAdd[0] = CDC_MODE_ACK;
						ring_buffer_put(&mosiBuff, packetDataToAdd, 1);

						_handleMisoAction(data[2]);
					}
					else
					{
						_reportMisoDataError("ERROR: MISO data byte 3 and 4 are not inverted.");
					}
				}
				else
				{
					_reportMisoDataError("ERROR: MISO data byte 1 and 2 are not as expected.");
				}
				
			}
		}
		if (currentTime > 65500)
		{
			_reportMisoDataError("ERROR: misoStopwatch will overflow.");
		}
	}
}

void _handleMisoAction(uint8_t action)
{
	uint8_t packetDataToAdd[1];
	switch (action)
	{
	case RADIO_CMD_CONFIRM: // received after any command
		printString("0");
		packetDataToAdd[0] = mosiData.mode; // do not change mode, just put it back in buffer
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		break;
	
	case RADIO_CMD_INIT: 
		printString("1");

		if(mosiLowLevelData.isEnabled)
		{ // "disable"
			printString("O");
			packetDataToAdd[0] = CDC_MODE_IDLE;
			mosiData.state = CDC_STATE_NOT_INITIALIZED;
		}
		else
		{ // "enable"
			printString("i");
			packetDataToAdd[0] = CDC_MODE_BUSY;
			mosiData.state = CDC_STATE_INITIALIZED;
		}
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		break;

	case RADIO_CMD_PLAY:
		printString("2");
		packetDataToAdd[0] = CDC_MODE_PLAY;
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		mosiData.state = CDC_STATE_PLAYING;
		break;

	case RADIO_CMD_PAUSE_MUTE:
		printString("3");
		packetDataToAdd[0] = CDC_MODE_BUSY;
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		mosiData.state = CDC_STATE_BUSY;
		break;

	case RADIO_CMD_NEXT:
		printString("4");
		packetDataToAdd[0] = mosiData.mode; // do not change mode, just put it back in buffer
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		
		addBluetoothAction(BT_ACT_NEXT);
		break;

	case RADIO_CMD_PREVIOUS:
		printString("5");
		packetDataToAdd[0] = mosiData.mode; // do not change mode, just put it back in buffer
		ring_buffer_put(&mosiBuff, packetDataToAdd, 1);
		
		addBluetoothAction(BT_ACT_PREV);
		break;

	case RADIO_CMD_FFW:
		printString("6");
	
		mosiData.isSendingDeckInfo = true;  //start sending DECK info at the very beginning
		mosiData.cdIndexCount = 0;
		mosiData.state = CDC_STATE_SENDING_CD_INFO;

		addBluetoothAction(BT_ACT_PLAY_PAUSE);
		break;

	case RADIO_CMD_FBW:
		printString("7");
		break;

	default:
		// what about just plain ignore?
		_reportMisoDataError("Unknown received MISO data.");
		break;
	}
}

void enableMisoDataHandler(bool state)
{
	if (state)
	{
		initMisoData();
	}
	else
	{
		stopMisoStopwatch();
	}
	misoData.isEnabled = state;
	misoPinChange = false;
}

void notifyPinChange(void)
{
	// this code is executed in interrupt routine
	misoPinChange = true;
}

void misoPinChangeChecker(void)
{
	// why? because I get a lot of interrupts because of spikes on DIN line
	// how: once interrupt set 'misoPinChange' variable to True, this function ensure that at majority of new read pin states is the same.
	uint8_t i;
	uint8_t readValue = 0;
	uint32_t capTime;

	//if (true)
	if (misoData.isEnabled)
	{
		if (misoPinChange)
		{
			capTime = getMisoStopwatchTimerValue();

			for (i = 0; i < DIN_CHANGE_NUM_OF_READS; i++)
			{
				readValue += LL_GPIO_IsInputPinSet(DIN_GPIO_Port, DIN_Pin);
			}

			if (readValue <= DIN_CHANGE_NUM_OF_READS_LOW_TRESHOLD)
			{
				if (misoData.lastPinState != 0)
				{
					misoData.capTime = capTime;
					misoPinChangeHandler(0);
				}
				// else: stable low detected, but previous state is also low.
			}
			else if (readValue >= DIN_CHANGE_NUM_OF_READS_HIGH_TRESHOLD)
			{
				if (misoData.lastPinState != 1)
				{
					misoData.capTime = capTime;
					misoPinChangeHandler(1);
				}
				// else: stable high detected, but previous state is also high.
			}
			misoPinChange = false;
		}
	}
}

void misoPinChangeHandler(uint8_t currentPinState)
{
	// time that has passed since first pin change (stored in misoData.capTime in misoPinChangeChecker) and end of this function.
	switch (misoData.state)
	{
	case DIN_IDLE:
		_IDLE_stateHandler(currentPinState);
		break;

	case DIN_SOF_HIGH:
		_SOF_HIGH_stateHandler(currentPinState);
		break;

	case DIN_SOF_LOW:
		_SOF_LOW_stateHandler(currentPinState);
		break;

	case DIN_DATA_HIGH:
	case DIN_DATA_LOW:
		_DATA_stateHandler(currentPinState);
		break;

	case DIN_EOF:
		_reportMisoDataError("ERROR: DIN_EOF state should not occur.");
		break;

	default:
		// invalid state!
		_reportMisoDataError("ERROR: Invalid misoData state machine state.");
		break;
	}

	misoData.lastPinState = currentPinState;
	
	// not completely accurate (processing time), but who cares.
	resetMisoStopwatch();
}

void _IDLE_stateHandler(uint8_t currentPinState)
{
	if (currentPinState == 1)
	{
		//printStringLn(" -> SOF_HIGH");
		startMisoStopwatch();
		misoData.state = DIN_SOF_HIGH;
		misoData.data = 0;
	}
	else
	{
		_reportMisoDataError("ERROR: LOW transition at IDLE state: ");
	}
}

void _SOF_HIGH_stateHandler(uint8_t currentPinState)
{
	if (currentPinState == 0)
	{
		if ((misoData.capTime > DIN_START_FIELD_HIGH_PULSE_MIN_US) && (misoData.capTime < DIN_START_FIELD_HIGH_PULSE_MAX_US))
		{
			// valid SOF HIGH pulse width
			//printStringLn(" -> SOF_LOW");
			misoData.state = DIN_SOF_LOW;
		}
		else
		{
			_reportMisoDataError("ERROR: invalid SOF HIGH pulse width: ");
		}
	}
	else
	{
		_reportMisoDataError("ERROR: HIGH transition at START_SEQUENCE_HIGH state: ");
	}
}

void _SOF_LOW_stateHandler(uint8_t currentPinState)
{
	if (currentPinState == 1)
	{
		if ((misoData.capTime > DIN_START_FIELD_LOW_PULSE_MIN_US) && (misoData.capTime < DIN_START_FIELD_LOW_PULSE_MAX_US))
		{
			// valid SOF LOW pulse width
			//printStringLn(" -> DATA");
			misoData.state = DIN_DATA_HIGH;
		}
		else
		{
			_reportMisoDataError("ERROR: invalid SOF LOW pulse width: ");
		}
	}
	else
	{
		_reportMisoDataError("ERROR: HIGH transition at START_SEQUENCE_HIGH state: ");
	}
}

void _DATA_stateHandler(uint8_t currentPinState)
{
	if (currentPinState == 1)
	{
		if (misoData.state == DIN_DATA_LOW)
		{
			// pin HIGH state, but previous data was a low pulse.
			// check timing of a previous low pulse and decide which logic bit value to add
			if ((misoData.capTime > DIN_DATA_LOGIC_1_LOW_PULSE_MIN_US) && (misoData.capTime < DIN_DATA_LOGIC_1_LOW_PULSE_MAX_US))
			{
				_addBitToData(true);
				misoData.state = DIN_DATA_HIGH;
			}
			else if ((misoData.capTime > DIN_DATA_LOGIC_0_LOW_PULSE_MIN_US) && (misoData.capTime < DIN_DATA_LOGIC_0_LOW_PULSE_MAX_US))
			{
				_addBitToData(false);
				misoData.state = DIN_DATA_HIGH;
			}
			else
			{
				_reportMisoDataError("ERROR: LOW pulse length not in range for logic 0 or 1: ");
			}
		}
		else // if (misoData.state != DIN_DATA_LOW)
		{
			_reportMisoDataError("ERROR: HIGH transition and not DIN_DATA_LOW state is invalid combination: ");
		}
	}
	else // LOW pin state
	{
		if (misoData.state == DIN_DATA_HIGH)
		{
			if ((misoData.capTime > DIN_DATA_HIGH_PULSE_MIN_US) && (misoData.capTime < DIN_DATA_HIGH_PULSE_MAX_US))
			{
				misoData.state = DIN_DATA_LOW;
			}
			else
			{
				_reportMisoDataError("ERROR: HIGH pulse length not in range for DATA_x state: ");
			}
		}
		else
		{
			_reportMisoDataError("ERROR: LOW transition in not DIN_DATA_HIGH state is invalid combination: ");
		}
	}
	return;
}

void _addBitToData(bool bit)
{
	// append each new bit on the MSB place (LSB first)
	misoData.data = (misoData.data >> 1);
	if (bit == true)
	{
		SET_BIT(misoData.data, (1 << 31));
	}
	else
	{
		CLEAR_BIT(misoData.data, (1 << 31));
	}
}

void _reportMisoDataError(char *data)
{
	stopMisoStopwatch();
	misoData.state = DIN_IDLE;
	misoData.isEnabled = false;

	ctrlErrorLed(true);
	printStringLn(data);
	printNumberLn(misoData.capTime, DEC);
	printNumberLn(misoData.data, BIN);
	printLn();
}

/**************************************************************************************************/
// other utility functions
/**************************************************************************************************/
uint8_t _invertByte(uint8_t data)
{
	data = ~data;

	return data;
}