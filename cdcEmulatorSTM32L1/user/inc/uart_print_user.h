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
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_PRINT_USER_H
#define __UART_PRINT_USER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "uart_print.h"

/* User implementation-specific includes -------------------------------------*/
#include "ring_buffer.h"

/* User implementation-specific defines --------------------------------------*/
#define UART_DRIVER USART1

#define UART_TX_TIMEOUT_MSEC 10
#define UART_RX_TIMEOUT_MSEC 10

#define UART_RX_BUFF_SIZE 128

    /* User implementation-specific variables ------------------------------------*/
    volatile rb_att_t uartRxBuff;

    void receiveByteIntoBuffer(void);

#ifdef __cplusplus
}
#endif

#endif
