#ifndef SF_WT2003M02_H
#define SF_WT2003M02_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

#define MP3_NUM_CMD_BYTES 11
#define MP3_NUM_NAME_BYTES 9

#define MP3_COMMAND_PLAY_INDEX_IN_ROOT 0xA0
#define MP3_COMMAND_PLAY_FILE_IN_ROOT 0xA1
#define MP3_COMMAND_PAUSE 0xAA
#define MP3_COMMAND_STOP 0xAB
#define MP3_COMMAND_NEXT 0xAC
#define MP3_COMMAND_PREVIOUS 0xAD
#define MP3_COMMAND_SET_VOLUME 0xAE
#define MP3_COMMAND_SET_PLAYMODE 0xAF
#define MP3_COMMAND_SET_PLAYMODE_SINGLE_NO_LOOP 0x00
#define MP3_COMMAND_SET_PLAYMODE_SINGLE_LOOP 0x01
#define MP3_COMMAND_SET_PLAYMODE_ALL_LOOP 0x02
#define MP3_COMMAND_SET_PLAYMODE_RANDOM 0x03

#define MP3_COMMAND_SET_EQ_MODE 0xB2
#define MP3_EQ_MODE_NORMAL 0x00
#define MP3_EQ_MODE_POP 0x01
#define MP3_EQ_MODE_ROCK 0x02
#define MP3_EQ_MODE_JAZZ 0x03
#define MP3_EQ_MODE_CLASSIC 0x04
#define MP3_EQ_MODE_BASS 0X05

#define MP3_COMMAND_GET_VOLUME 0xC1
#define MP3_COMMAND_GET_CURRENT_STATE 0xC2
#define MP3_STATUS_PLAY 0x01
#define MP3_STATUS_STOP 0x02
#define MP3_STATUS_PAUSE 0x03
#define MP3_COMMAND_GET_SONG_COUNT 0xC3
#define MP3_COMMAND_GET_FILE_PLAYING 0xC9
#define MP3_COMMAND_GET_SONG_NAME_PLAYING 0xCB



#define MP3_START_CODE 0x7E
#define MP3_END_CODE 0xEF

class WT2003M02 {
private:

protected:
    SoftwareSerial *_serialPort;

public:
    uint8_t commandBytes[MP3_NUM_CMD_BYTES];
    uint8_t songName[MP3_NUM_NAME_BYTES];
    uint8_t delayAfterCommand = 4;

    WT2003M02();

    void begin(uint8_t rx, uint8_t tx);

    uint8_t setPlaymodeSingleNoLoop(void);

    uint8_t setPlaymodeSingleLoop(void);

    uint8_t setPlaymodeAllLoop(void);

    uint8_t setPlaymodeRandom(void);

    uint16_t getSongCount(void);

    uint8_t *getSongName(void);

    uint8_t playTrackNumber(uint8_t trackNumber);

    uint8_t playFileName(uint8_t *fileName);

    uint8_t playFileNumber(uint16_t fileNumber);

    uint8_t setVolume(uint8_t volumeLevel);

    uint8_t getVolume(void);

    uint8_t setEQ(uint8_t eqType);

    uint8_t getStatus(void);

    uint8_t pause(void);

    uint8_t next(void);

    uint8_t prev(void);

    uint8_t stop(void);

    void sendCommand(uint8_t commandLength);

    uint8_t getResponse(void);

    uint16_t getTwoByteResponse(void);

    bool responseAvailable(void);

    void clearBuffer(void);
};

#endif