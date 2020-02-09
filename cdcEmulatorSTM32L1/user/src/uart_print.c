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
 * 
 * Follow the instructions in uart_print_user.h
 */

// private includes
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "uart_print.h"

/****************************************************************************************/
/* PRINT/WRITE FUNCTIONS */
/****************************************************************************************/
/*
	Send/print character/string. 
	Printable data for viewing on terminal.
	Call this function:		printString("test data");
	Data must be string.
*/
void printString(char *data)
{
	sendData((uint8_t *)data, (uint32_t)strlen(data));
}

/*
	Send/print unsigned or signed number. 
	Printable data for viewing on terminal.
	Call this function:		printNumber(number, DEC);		printNumber(2246, DEC);	
	Base: DEC, HEX, OCT, BIN
	Data must be number, int32_t.
*/
void printNumber(int32_t number, uint8_t base)
{
	if (number < 0)
	{
		printString("-");
		number = -number;
	}
	printUnsignedNumber(number, base);
}

//print WITH new line and carriage return
void printStringLn(char *data)
{
	printString(data);
	printLn();
}

/*
	Send/print unsigned or signed number. 
	Printable data for viewing on terminal.
	Call this function:		printNumber(number, DEC);		printNumber(2246, DEC);	
	Base: DEC, HEX, OCT, BIN
	Data must be number, int32_t.
*/
void printNumberLn(int32_t number, uint8_t base)
{
	printNumber(number, base);
	printLn();
}

void printLn()
{
	printString("\r\n");
}

/*
	Send raw data. 
	Not "printable" data.
	Call this function:		writeData(&data, sizeof(data));
	Data can be any type.
*/
void writeData(uint8_t *data, uint8_t dataSize)
{
	sendData((uint8_t *)data, (uint32_t)dataSize);
}

/*
	This is "private" function. It is used by other functions like: printNumber(int32_t number, uint8_t base). 
	However, it can be used by user.
	Send/print unsigned number. 
	Printable data for viewing on terminal.
	Call this function:		printUnsignedNumber(number, DEC);		printUnsignedNumber(2246, DEC);	
	Base: DEC, HEX, OCT, BIN
	Data must be number, int32_t.
*/
void printUnsignedNumber(uint32_t n, uint8_t base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];
	unsigned long m;
	char c;
	*str = '\0';

	//prevent crash if called with base == 1
	if (base < 2)
		base = 10;

	do
	{
		m = n;
		n /= base;
		c = m - base * n;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while (n);

	printString(str);
}

// pay attention on "double" type range
// ? - max 6 digits after decimal point?
void printFloat(double number)
{
	char float_as_string[20];
	sprintf(float_as_string, "%f", number);
	printString(float_as_string);
}

void printFloatLn(double number)
{
	printFloat(number);
	printLn();
}
