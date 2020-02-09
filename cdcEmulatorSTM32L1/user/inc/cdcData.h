/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CDC_H
#define __CDC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************************************/
// USER SETTINGS (in HEX values)
/*************************************************************************************************/
#define CDC_DISPLAY_ENABLED_CD 1 // 1 - 6 // not shown - no deck info was sent or recognised by radio
#define CDC_DISPLAY_TRACK_NUMBER 0x42
#define CDC_DISPLAY_TIME_MIN 0x22
#define CDC_DISPLAY_TIME_SEC 0x8

/*************************************************************************************************/
// CDC DATA IN/OUT TIMINGS
/*************************************************************************************************/
// DOUT - DATA OUT
#define DOUT_PACKET_BYTES_NUMBER 8     // [bytes] - number of bytes in each packet
#define DOUT_PACKET_BYTES_DELAY_US 992 // [us] - delay between bytes in a packet (varies in reality! +/- 10 us)
#define DOUT_PACKET_DELAY_US 50000     // [us] - delay between packets (varies a lot in reality)

// DIN - DATA IN
#define DIN_PACKET_BYTES_NUMBER 4 // [bytes] - number of bytes
// start pulse
#define DIN_START_FIELD_HIGH_PULSE_US 9000 // [us] - measured
#define DIN_START_FIELD_HIGH_PULSE_MIN_US 8500
#define DIN_START_FIELD_HIGH_PULSE_MAX_US 9500
#define DIN_START_FIELD_LOW_PULSE_US 4500 // [us] - measured
#define DIN_START_FIELD_LOW_PULSE_MIN_US 4000
#define DIN_START_FIELD_LOW_PULSE_MAX_US 5000
// logic 0 and 1
#define DIN_DATA_HIGH_PULSE_US 558 // [us] - measured
#define DIN_DATA_HIGH_PULSE_MIN_US 480
#define DIN_DATA_HIGH_PULSE_MAX_US 640
#define DIN_DATA_LOGIC_1_LOW_PULSE_US 1684 // [us] - measured
#define DIN_DATA_LOGIC_1_LOW_PULSE_MIN_US 1600
#define DIN_DATA_LOGIC_1_LOW_PULSE_MAX_US 1760
#define DIN_DATA_LOGIC_0_LOW_PULSE_US DIN_DATA_HIGH_PULSE_US
#define DIN_DATA_LOGIC_0_LOW_PULSE_MIN_US DIN_DATA_HIGH_PULSE_MIN_US
#define DIN_DATA_LOGIC_0_LOW_PULSE_MAX_US DIN_DATA_HIGH_PULSE_MAX_US
// EOF
#define DIN_EOF_US DIN_DATA_LOGIC_1_LOW_PULSE_MAX_US

// DIP pin debounce settings
#define DIN_CHANGE_NUM_OF_READS 6
#define DIN_CHANGE_NUM_OF_READS_LOW_TRESHOLD 1
#define DIN_CHANGE_NUM_OF_READS_HIGH_TRESHOLD 5

/*************************************************************************************************/
// CDC DATA OUT
/*************************************************************************************************/
#define DOUT_BYTE_MODE_INDEX 0
#define DOUT_BYTE_CD_INDEX 1
#define DOUT_BYTE_TRACK_INDEX 2
#define DOUT_BYTE_TIME_MSB_INDEX 3
#define DOUT_BYTE_TIME_LSB_INDEX 4
#define DOUT_BYTE_FRAME_1_INDEX 5
#define DOUT_BYTE_FRAME_2_INDEX 6
#define DOUT_BYTE_FRAME_3_INDEX 7

  // MODE (byte 0) - mode/communication ???
  typedef enum
  {
    CDC_MODE_IDLE = 0xF4,
    CDC_MODE_ACK = 0x94,
    CDC_MODE_BUSY = 0xB4,
    CDC_MODE_PLAY = 0x34
  } cdcMode_t;

  // FRAME 2 (byte 6) - cdc state
  typedef enum
  {
    // 0x6F - appeared, but don't know why
    CDC_STATE_NOT_INITIALIZED = 0x8F,
    CDC_STATE_BUSY = 0xAF,
    CDC_STATE_FAST_FW_BW = 0xBF,
    CDC_STATE_MUTE = 0xDF,
    CDC_STATE_PLAYING = 0xCF,
    CDC_STATE_INITIALIZED = 0xEF,
    CDC_STATE_SENDING_CD_INFO = 0xFF
  } cdcState_t;

  /*************************************************************************************************/
  // CDC DATA IN
  /*************************************************************************************************/
  typedef enum
  {
    // 0x6F - appeared, but don't know why
    RADIO_CMD_INIT = 0x08,
    RADIO_CMD_CONFIRM = 0x28,

    RADIO_CMD_PLAY = 0x27,
    RADIO_CMD_PAUSE_MUTE = 0xC,
    RADIO_CMD_NEXT = 0x1F,
    RADIO_CMD_PREVIOUS = 0x1E,
    RADIO_CMD_FFW = 0x1B,
    RADIO_CMD_FBW = 0x1A,
  } dinCommands_t;

#define DIN_BYTE_0_VALUE 0xCA
#define DIN_BYTE_1_VALUE 0x34

#ifdef __cplusplus
}
#endif

#endif
