
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_IO_H
#define __UART_IO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "ring_buffer.h"

#define UCMD_RX_DATA_SIZE 4 // [bytes] - size of any incoming packet in bytes

#define UCMD_CTRL_LED_TOGGLE 'L' // byte1
  #define UCMD_CTRL_LED_TOGGLE_RED 'r' // byte2
  #define UCMD_CTRL_LED_TOGGLE_GREEN 'g' // byte2
  #define UCMD_CTRL_LED_TOGGLE_BLUE 'b' // byte2
  

#define UCMD_MFD_ACTION 'R'         // byte1
  #define UCMD_MFD_CTRL_ENABLE 'E' // byte2 enable miso/mosi data handling
  #define UCMD_MFD_CTRL_DISABLE 'D' // byte2 disable miso/mosi data handling
  #define UCMD_MFD_CTRL_MOSI_DATA 'O' // byte2
    #define UCMD_MFD_CTRL_MOSI_DATA_START 'S' // byte3
    #define UCMD_MFD_CTRL_MOSI_DATA_STOP 'X' // byte3
  #define UCMD_MFD_CTRL_MISO_DATA 'I' // byte2
    #define UCMD_MFD_CTRL_MISO_DATA_START 'S' // byte3
    #define UCMD_MFD_CTRL_MISO_DATA_STOP 'X' // byte3
  
  #define UCMD_MFD_SET_TRACK 'T'      // byte2

#define UCMD_TEST 'T'               // byte1
  #define UCMD_TEST_SEND_SPI_BYTE 'B' // byte2, data=// byte3

  void uartRxDataHandler(void);

#ifdef __cplusplus
}
#endif

#endif
