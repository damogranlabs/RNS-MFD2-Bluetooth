/**
 ******************************************************************************
 * File Name          : uart_print_user
 * Description        : This file provides simple intarface to send strings,
 * 											numbers over UART.
 * @date    07-Oct-2019
 * @author  Domen Jurkovic
 * @version v1.1
 *
 * Copyright (c) 2019, damogranlabs.com
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @source  http://damogranlabs.com/
 *          https://github.com/damogranlabs
 * 
 * How to use:
 * 1. Include correct files. See HARDWARE INIT FUNCTIONS
 * 2. Fill sendData() and receiveByte()
 */
#include "uart_print.h"
#include "uart_print_user.h"

/****************************************************************************************/
/* HARDWARE INIT FUNCTIONS - change accordingly to your HW */
/****************************************************************************************/
#include "usart.h"

#include "io.h"
#include "stm32l1xx_it.h"

void initUart(void){
	// init cdcEmulator <-> PC UART
	NVIC_ClearPendingIRQ(USART1_IRQn);

	// Init RX ring buffer
  if (ring_buffer_init(&uartRxBuff, UART_RX_BUFF_SIZE) != RB_OK)
	{
    Error_Handler();
	}

	// init UART driver
  MX_USART1_UART_Init();
  LL_USART_EnableIT_RXNE(UART_DRIVER);
  
}


void sendData(uint8_t *data, uint32_t numOfBytes){
	uint32_t byteNum = 0;
	uint32_t endTime = LL_GetTick() + UART_TX_TIMEOUT_MSEC;


	for(byteNum=0; byteNum < numOfBytes; byteNum++)
	{
		// wait until UART TX buffer is not empty
		while(!LL_USART_IsActiveFlag_TXE(UART_DRIVER))
		{
			if(LL_GetTick() > endTime)
			{
				Error_Handler();
			}
		}

		// send data
		LL_USART_TransmitData8(UART_DRIVER, *(data + byteNum));
	
		// wait until TX buffer is not empty
		while(!LL_USART_IsActiveFlag_TXE(UART_DRIVER))
		{
			if(LL_GetTick() > endTime)
			{
				Error_Handler();
			}
		}
	}
}

uint8_t receiveByte(void)
{
	uint8_t byte = LL_USART_ReceiveData8(UART_DRIVER);
	return byte;
}

void receiveByteIntoBuffer(void)
{
	uint8_t data = 0;
	data = LL_USART_ReceiveData8(UART_DRIVER);
	if(ring_buffer_put(&uartRxBuff, &data, 1) != RB_OK){
		Error_Handler();
	}

}
