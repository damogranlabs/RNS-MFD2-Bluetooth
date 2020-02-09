
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MFD_IO_H
#define __MFD_IO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "stm32l1xx_ll_gpio.h"

#include "ring_buffer.h"
#include "cdcData.h"

#define SPI_DRIVER SPI1

#define MOSI_DATA_BUFFER_SIZE 16

  typedef enum
  {
    DIN_IDLE = 0,
    DIN_SOF_HIGH,
    DIN_SOF_LOW,
    DIN_DATA_HIGH,
    DIN_DATA_LOW,
    DIN_EOF
  } misoState_t;

  typedef struct
  {
    bool isEnabled;

    misoState_t state; // MFD GPIO MISO state machine state
    uint32_t data;
    uint32_t capTime;
    uint8_t lastPinState;
  } misoData_t;

  typedef struct
  {
    bool isEnabled;
    
    bool isSendingPacket;  
    uint32_t currentByteNum;

    uint32_t nextByteTimestamp;
  } mosiLowLevelData_t;

  typedef struct
  {
    cdcMode_t mode;
    uint8_t cd;
    uint8_t track;
    uint16_t time;
    cdcState_t state;

    bool isSendingDeckInfo;
    uint8_t cdIndexCount; // when sending deck info, this variable tracks index of a current CD (that data is sent to radio)
  } mosiData_t;

  volatile bool misoPinChange;
  volatile misoData_t misoData;
  
  rb_att_t mosiBuff;
  mosiData_t mosiData;
  mosiLowLevelData_t mosiLowLevelData;


  void initMisoData(void);
  void misoDataHandler(void);
  void enableMisoDataHandler(bool state);
  
  void notifyPinChange(void);
  void misoPinChangeChecker(void);
  void misoPinChangeHandler(uint8_t newPinState);
  void misoPinInterruptHanderCallback(void);

  void initSpiDriver(void);
  void initMosiBuffer(void);
  void initMosiData(void);
  void mosiDataHandler(void);
  void enableMosiDataHandler(bool state);
  
#ifdef __cplusplus
}
#endif

#endif
