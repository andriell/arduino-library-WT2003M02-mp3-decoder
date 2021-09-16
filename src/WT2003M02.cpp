#include "WT2003M02.h"

WT2003M02::WT2003M02() {
}

void WT2003M02::begin(uint8_t rx, uint8_t tx) {
    _serialPort = new SoftwareSerial(rx, tx);
    _serialPort->begin(9600);
    for (uint8_t indi = 0; indi < MP3_NUM_CMD_BYTES; indi++) {
        commandBytes[indi] = 0x00;
    }
}

uint8_t WT2003M02::setPlaymodeSingleNoLoop(void) {
    commandBytes[0] = MP3_COMMAND_SET_PLAYMODE;
    commandBytes[1] = MP3_COMMAND_SET_PLAYMODE_SINGLE_NO_LOOP;
    sendCommand(2);
    return (getResponse());
}

uint8_t WT2003M02::setPlaymodeSingleLoop(void) {
    commandBytes[0] = MP3_COMMAND_SET_PLAYMODE;
    commandBytes[1] = MP3_COMMAND_SET_PLAYMODE_SINGLE_LOOP;
    sendCommand(2);
    return (getResponse());
}

uint8_t WT2003M02::setPlaymodeAllLoop(void) {
    commandBytes[0] = MP3_COMMAND_SET_PLAYMODE;
    commandBytes[1] = MP3_COMMAND_SET_PLAYMODE_ALL_LOOP;
    sendCommand(2);
    return (getResponse());
}

uint8_t WT2003M02::setPlaymodeRandom(void) {
    commandBytes[0] = MP3_COMMAND_SET_PLAYMODE;
    commandBytes[1] = MP3_COMMAND_SET_PLAYMODE_RANDOM;
    sendCommand(2);
    return (getResponse());
}

uint16_t WT2003M02::getSongCount(void) {
    commandBytes[0] = MP3_COMMAND_GET_SONG_COUNT;
    sendCommand(1);

    if (responseAvailable() == false)
        return (0); //Timeout

    //Get three byte response
    uint16_t response = 0xFFFF;
    uint8_t i = 0;

    while (_serialPort->available()) {
        uint8_t incoming = _serialPort->read();
        if (i == 0); //This is throw away value 0xC5
        else if (i == 1)
            response = (incoming << 8); //MSB
        else if (i == 2)
            response |= incoming; //LSB

        i++;
        delay(1); //At 9600bps 1 byte takes 0.8ms
    }
    return (response);
}

uint8_t *WT2003M02::getSongName() {
    commandBytes[0] = MP3_COMMAND_GET_SONG_NAME_PLAYING;
    sendCommand(1);

    if (responseAvailable() == false) {
        songName[0] = 'E';
        songName[1] = 'r';
        songName[2] = 'r';
        songName[3] = 'o';
        songName[4] = 'r';
        songName[5] = '\0';
        return songName;
    }

    //Get 9 byte response
    uint8_t i = 0;

    while (_serialPort->available()) {
        uint8_t incoming = _serialPort->read();
        if (i == 0); //This is throw away value 0xCB
        else if (i < 9)
            songName[i - 1] = incoming;
        i++;
        delay(1); //At 9600bps 1 byte takes 0.8ms
    }
    songName[8] = '\0'; //Terminate this string
    return songName;

}

uint16_t WT2003M02::getFileNumber() {
    getSongName();
    uint16_t r = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (songName[i] < '0' || songName[i] > '9') {
            break;
        }
        r = r * 10 + (songName[i] - '0');
    }
    return r;
}

uint8_t WT2003M02::playTrackNumber(uint8_t trackNumber) {
    commandBytes[0] = MP3_COMMAND_PLAY_INDEX_IN_ROOT;
    commandBytes[1] = trackNumber >> 8;   //MSB
    commandBytes[2] = trackNumber & 0xFF; //LSB
    sendCommand(3);
    return (getResponse());
}

uint8_t WT2003M02::playFileName(uint8_t *fileName) {
    commandBytes[0] = MP3_COMMAND_PLAY_FILE_IN_ROOT;
    commandBytes[1] = fileName[0];
    commandBytes[2] = fileName[1];
    commandBytes[3] = fileName[2];
    commandBytes[4] = fileName[3];
    sendCommand(5);
    return (getResponse());
}

uint8_t WT2003M02::playFileNumber(uint16_t fileNumber) {
    commandBytes[0] = MP3_COMMAND_PLAY_FILE_IN_ROOT;
    commandBytes[1] = '0' + (fileNumber / 1000);
    fileNumber %= 1000;
    commandBytes[2] = '0' + (fileNumber / 100);
    fileNumber %= 100;
    commandBytes[3] = '0' + (fileNumber / 10);
    fileNumber %= 10;
    commandBytes[4] = '0' + fileNumber;
    sendCommand(5);
    return (getResponse());
}

uint8_t WT2003M02::setVolume(uint8_t volumeLevel) {
    if (volumeLevel > 31)
        volumeLevel = 31; //Error check
    commandBytes[0] = MP3_COMMAND_SET_VOLUME;
    commandBytes[1] = volumeLevel;
    sendCommand(2);
    return (getResponse());
}

uint8_t WT2003M02::getVolume(void) {
    commandBytes[0] = MP3_COMMAND_GET_VOLUME;
    sendCommand(1);

    //Get two byte response
    uint16_t volLevel = getTwoByteResponse();

    //First byte is 0xC1, second bye is volume level
    return (volLevel & 0xFF);
}

uint8_t WT2003M02::setEQ(uint8_t eqType) {
    if (eqType > 5)
        eqType = 0; //Error check. Set to normal by default
    commandBytes[0] = MP3_COMMAND_SET_EQ_MODE;
    commandBytes[1] = eqType;
    sendCommand(2);
    return (getResponse());
}

uint8_t WT2003M02::getStatus(void) {
    commandBytes[0] = MP3_COMMAND_GET_CURRENT_STATE;
    sendCommand(1);
    return (getTwoByteResponse() & 0xFF);
}

uint8_t WT2003M02::pause(void) {
    commandBytes[0] = MP3_COMMAND_PAUSE;
    sendCommand(1);
    return (getResponse());
}

uint8_t WT2003M02::next(void) {
    commandBytes[0] = MP3_COMMAND_NEXT;
    sendCommand(1);
    return (getResponse());
}

uint8_t WT2003M02::prev(void) {
    commandBytes[0] = MP3_COMMAND_PREVIOUS;
    sendCommand(1);
    return (getResponse());
}

uint8_t WT2003M02::stop(void) {
    commandBytes[0] = MP3_COMMAND_STOP;
    sendCommand(1);
    return (getResponse());
}

void WT2003M02::sendCommand(uint8_t commandLength) {
    clearBuffer(); //Clear anything in the buffer

    _serialPort->write(MP3_START_CODE);
    _serialPort->write(commandLength + 2); //Add one byte for 'length', one for CRC

    //Begin sending command bytes while calc'ing CRC
    byte crc = commandLength + 2;
    for (byte x = 0; x < commandLength; x++) //Length + command code + parameter
    {
        _serialPort->write(commandBytes[x]); //Send this byte
        crc += commandBytes[x];          //Add this byte to the CRC
    }

    _serialPort->write(crc); //Send CRC
    _serialPort->write(MP3_END_CODE);
    delay(delayAfterCommand);
}

uint8_t WT2003M02::getResponse(void) {
    if (responseAvailable() == false)
        return (0xFF); //Timeout

    uint8_t response = 0xFE;
    uint8_t i = 0;


    while (_serialPort->available()) {
        if (i++ == 0)
            response = _serialPort->read();
    }

    return (response);

}

uint16_t WT2003M02::getTwoByteResponse(void) {
    if (responseAvailable() == false)
        return (0xFF); //Timeout

    uint16_t response = 0xFFFF;
    uint8_t i = 0;

    while (_serialPort->available()) {
        uint8_t incoming = _serialPort->read();

        if (i == 0)
            response = incoming << 8; //MSB
        else if (i == 1)
            response |= incoming; //LSB

        i++;
        delay(1); //At 9600bps 1 byte takes 0.8ms
    }

    return (response);

}

bool WT2003M02::responseAvailable(void) {
    uint8_t counter = 0;

    while (_serialPort->available() == false) {
        delay(1);

        if (counter++ > 250) {
            return (false); //Timeout
        }
    }
    return (true);

}

void WT2003M02::clearBuffer(void) {
    while (_serialPort->available()) {
        _serialPort->read();
        delay(1); //1 byte at 9600bps should take 1ms
    }
}