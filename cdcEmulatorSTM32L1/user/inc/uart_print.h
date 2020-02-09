/**
 ******************************************************************************
 * File Name          : uart_print
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
#ifndef __UART_PRINT_H
#define __UART_PRINT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

//uint8_t base:
#define DEC 10
#define BIN 2
#define HEX 16
#define OCT 8

    /****************************************************************************************/
    /* COMMUNICATION FUNCTIONS - change in uart_print_user.c/.h file accordingly to your HW */
    /****************************************************************************************/
    void initUart(void);
    void sendData(uint8_t *data, uint32_t numOfBytes);
    uint8_t receiveByte(void);

    /****************************************************************************************/
    /* PRINT/WRITE FUNCTIONS */
    /****************************************************************************************/
    //print WITHOUT new line and carriage return
    void printString(char *data);                   //send/print string overserial.
    void printNumber(int32_t number, uint8_t base); //send/print SINGED/UNSIGNED int32_t number
    void printFloat(double number);

    //print WITH new line and carriage return
    void printStringLn(char *data);                   //send/print string.
    void printNumberLn(int32_t number, uint8_t base); //send/print SINGED/UNSIGNED number.
    void printLn(void);                               //print new line and carriage return
    void printFloatLn(double number);

    //send raw data, any type.
    void writeData(uint8_t *data, uint8_t dataSize);

    //"private" function. Can be used if needed.
    void printUnsignedNumber(uint32_t n, uint8_t base); //send/print UNSIGNED uint32_t.

#ifdef __cplusplus
}
#endif

#endif
